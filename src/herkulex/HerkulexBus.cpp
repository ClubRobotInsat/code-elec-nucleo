#include "HerkulexBus.h"

namespace herkulex {

	/* --------------------------------------------------------------------------------------------
	 * Constructeur (explicit)
	 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin.
	 * Log son activite sur le Serial pointe par log.
	 * Le baudrate de la com. peut etre specifie.
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::Bus(PinName txPin, PinName rxPin, uint32_t baudrate, float flush_frequency)
	        : _ticker_flush()
	        , _callback_waiting(false)
	        , _write_done(true)
	        , _ser(txPin, rxPin, baudrate)
	        , _read_callback(Callback<void(int)>(this, &Bus::cbInterpretBuffer))
	        , _write_callback(Callback<void(int)>(this, &Bus::cbWriteDone))
	        , _buffer_write_data()
	        , _buffer_write_length()
	        , _total_write_length(0)
	        , _data_written(nullptr) {
		_ticker_flush.attach(Callback<void()>(this, &Bus::flushOneMessage), flush_frequency);
	}

	/* --------------------------------------------------------------------------------------------
	 * Destructeur
	 * Desalloue le Serial utilise par le bus
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::~Bus() {
		// debug("Destruction du bus");
	}

	void Bus::flushOneMessage() {
		if(not _write_done or _buffer_write_data.empty()) {
			return;
		}
		uint8_t message_length = 0;
		_buffer_write_length.pop(message_length);
		uint8_t* message;
		_buffer_write_data.pop(message);
		_ser.write(message, message_length, _write_callback, SERIAL_EVENT_TX_ALL);
		_write_done = false;
		_total_write_length -= message_length;
	}

	void Bus::flush() {
		if(not _write_done) {
			return;
		}
		if(_total_write_length == 0) {
			return;
		}
		uint8_t* message = new uint8_t[_total_write_length];
		uint32_t index = 0;
		while(!_buffer_write_length.empty()) {
			uint8_t length;
			uint8_t* data;
			_buffer_write_length.pop(length);
			_buffer_write_data.pop(data);
			// debug("Sending : (%d) [", length);
			for(uint8_t i = 0; i < length; i++) {
				message[index + i] = data[i];
			}
			delete data;
			index += length;
		}
		_ser.write(message, _total_write_length, _write_callback, SERIAL_EVENT_TX_ALL);
		_write_done = false;
		_total_write_length = 0;
		_data_written = message;
	}

	void Bus::cbWriteDone(int e) {
		_write_done = true;
		delete[] _data_written;
	}

	bool Bus::needFlush() {
		return _buffer_write_data.full();
	}

	/* --------------------------------------------------------------------------------------------
	 * write
	 * Prend un buffer (data), de taille fixe (length), passe par addresse et l'ecoule sur le bus
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::write(uint8_t* data, uint8_t length) {
		for(uint8_t i = 0; i < length; i++) {
			//_ser.putc(data[i]);
		}

		while(not _write_done) {
			// debug("Waiting for previous write\n\r");
			wait_ms(1000);
		}

		_write_done = false;

		if(data == nullptr) {
			// debug("Erreur nullptr data write\n\r");
		}
		// debug("Write call\n\r");
		_ser.write(data, length, _write_callback, SERIAL_EVENT_TX_ALL);
	}

	/* --------------------------------------------------------------------------------------------
	 * sendMsg
	 * Construit un message pour les servos, et l'envoi immediatement sur le bus.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendMsg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t* data, const uint8_t length) {
		uint8_t total_length = length + constants::Size::MinPacketSize;
		uint8_t index = 0;

		// NEW
		uint8_t* txBuf = new uint8_t(total_length);

		txBuf[0] = constants::header;
		txBuf[1] = constants::header;
		txBuf[2] = total_length;
		txBuf[3] = id;
		txBuf[4] = static_cast<uint8_t>(cmd);

		// Start construction of checksum
		txBuf[5] = txBuf[2] ^ txBuf[3] ^ txBuf[4];
		txBuf[6] = 0;

		if(data != nullptr) {
			for(index = 0; index < length; ++index) {
				txBuf[constants::Size::MinPacketSize + index] = data[index];
				// Iteratively construct the checksum
				txBuf[5] = txBuf[5] ^ data[index];
			}
		}
		txBuf[6] = (~txBuf[5]) & 0xFE;
		txBuf[5] = txBuf[5] & 0xFE;

		_buffer_write_length.push(total_length);
		_buffer_write_data.push(txBuf);
		_total_write_length += total_length;

		// DELETE
		// delete txBuf;
	}


	/* --------------------------------------------------------------------------------------------
	 * cbInterpretBuffer
	 * Callback pour lire et interpreter un message recu. Globalement, switch sur le type de
	 * message et appelle le parser correspondant au message.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::cbInterpretBuffer(int event) {
		// debug("Callback \n\r");
		if(_buffer[3] == _expected_reply_id && _buffer[4] == _expected_reply_cmd) {
			switch(_buffer[4]) {
				// If we received an addr read ack
				case constants::CMD::fromServo::EEPReadAck:
					break;
				case constants::CMD::fromServo::RAMReadAck:
					// debug("Received RAMREAD\n\r");
					parseAddrMsg();
					break;

				// If we received a status read ack
				case constants::CMD::fromServo::StatAck:
					parseStatMsg();
					break;

				// Do nothing for other replies
				case constants::CMD::fromServo::EEPWriteAck:
					break;
				case constants::CMD::fromServo::RAMWriteAck:
					break;
				case constants::CMD::fromServo::IJOGAck:
					break;
				case constants::CMD::fromServo::SJOGAck:
					break;
				case constants::CMD::fromServo::RollbackAck:
					break;
				case constants::CMD::fromServo::RebootAck:
					break;

				default:
					// debug("Recu un message servo avec une mauvaise CMD\n");
					break;
			}
			_callback_waiting = false; // TODO - [supprimer], ou utiliser avec un timeout
		} else {
			// debug("Bad ID");
		}
	}

	void Bus::sendDebugMessage() {
		// debug("Sending _log->printf... \n\r");
		uint8_t txBuf[9];
		txBuf[0] = 0xFF;
		txBuf[1] = 0xFF;
		txBuf[2] = 0x07;
		txBuf[3] = 0xFD;
		txBuf[4] = 0x07;
		txBuf[5] = 0xFC;
		txBuf[6] = 0x02;
		txBuf[7] = 0x00;
		txBuf[8] = 0x40;
		write(txBuf, 9);
	}
	/* --------------------------------------------------------------------------------------------
	 * sendEEPWriteMsg
	 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendEEPWriteMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendEEPWriteMsg avec une longueur erronee !\n");
		} else {
			// NEW
			uint8_t* data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			sendMsg(id, constants::CMD::toServo::EEPWrite, data, (2 + len));

			// DELETE
			// delete data;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendEEPReadMsg
	 * Construit un de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!!
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendEEPReadMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendEEPReadMsg avec une longueur erronee !\n");
		} else {
			uint8_t data[2];

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::EEPRead, data, 2);
		}
	}


	/* --------------------------------------------------------------------------------------------
	 * sendRAMWriteMsg
	 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRAMWriteMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} else {
			// NEW
			uint8_t* data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			sendMsg(id, constants::CMD::toServo::RAMWrite, data, 2 + len);

			// DELETE
			// delete data;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRAMReadMsg
	 * Construit un de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!!
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} else {
			// NEW
			uint8_t* data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::RAMRead, data, 2);

			// DELETE
			// delete data;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendIJOGMsg
	 * Envoi d'une commande IJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex :
	 * -> champ SET d'un paquet IJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendIJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		// NEW
		uint8_t* data = new uint8_t(5);

		data[0] = (jogValue & 0xff);
		data[1] = (jogValue & 0xff00) >> 8;
		data[2] = set;
		data[3] = id;
		data[4] = playtime;

		sendMsg(id, constants::CMD::toServo::IJOG, data, 5);

		// DELETE
		// delete data;
	}

	/* --------------------------------------------------------------------------------------------
	 * sendSJOGMsg
	 * Envoi d'une commande SJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex :
	 * -> champ SET d'un paquet SJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendSJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		// NEW
		uint8_t* data = new uint8_t(5);

		data[0] = playtime;
		data[1] = (jogValue & 0x00ff);
		data[2] = (jogValue & 0xff00) >> 8;
		data[3] = set;
		data[4] = id;

		sendMsg(id, constants::CMD::toServo::SJOG, data, 5);

		// DELETE
		// delete data;
	}

	/* --------------------------------------------------------------------------------------------
	 * sendStatMsg
	 * Envoi un message de demande de status
	 !!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour
	 !!! la requete de status
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendStatMsg(uint8_t id) {
		// NEW
		uint8_t* data = new uint8_t(2);

		data[0] = 0;
		data[1] = 0;

		sendMsg(id, constants::CMD::toServo::Stat, data, 2);

		// DELETE
		// delete data;
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRollbackMsg
	 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRollbackMsg(uint8_t id, bool skipIDRollback, bool skipBaudrateRollback) {
		// NEW
		uint8_t* data = new uint8_t(2);

		data[0] = (skipIDRollback ? 1 : 0);
		data[1] = (skipBaudrateRollback ? 1 : 0);

		sendMsg(id, constants::CMD::toServo::Rollback, data, 2);

		// DELETE
		// delete data;
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRebootMsg
	 * Envoi un message de reboot
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRebootMsg(uint8_t id) {
		sendMsg(id, constants::CMD::toServo::Reboot);
	}

	/* --------------------------------------------------------------------------------------------
	 * readEEPAddr
	 * Read at a specified address in a servo's EEP.
	 * Callback prototype example :
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0);
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readEEPAddr(uint8_t id,
	                      constants::EEPAddr::EEPAddrEnum addr,
	                      uint8_t len,
	                      Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>* callback) {
		sendEEPReadMsg(id, addr, len);
		_callback_read_addr = callback;
		_expected_reply_id = id;
		_expected_reply_cmd = constants::CMD::fromServo::EEPReadAck;
	}

	/* --------------------------------------------------------------------------------------------
	 * readRAMAddr
	 * Read at a specified address in a servo's RAM.
	 * Callback prototype example :
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0);
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readRAMAddr(uint8_t id,
	                      constants::RAMAddr::RAMAddrEnum addr,
	                      uint8_t len,
	                      Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>* callback) {
		_ser.read(_buffer, (uint8_t)13, _read_callback, SERIAL_EVENT_RX_COMPLETE);
		sendRAMReadMsg(id, addr, len);
		_callback_read_addr = callback;
		_expected_reply_id = id;
		_expected_reply_cmd = constants::CMD::fromServo::RAMReadAck;
		debug("Read called\n\r");
	}

	/* --------------------------------------------------------------------------------------------
	 * readStat
	 * Read the status of a servo.
	 * Callback prototype example :
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail);
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readStat(uint8_t id, Callback<void(uint8_t, uint8_t, uint8_t)>* callback) {
		sendStatMsg(id);
		_callback_read_stat = callback;
		_expected_reply_id = id;
		_expected_reply_cmd = constants::CMD::fromServo::StatAck;
	}

	/* --------------------------------------------------------------------------------------------
	 * parseAddrMsg
	 * This function is an helper. It should not be called outside of cbInterpretBuffer.
	 * Parses a addr read (EEP or RAM) ack message received from a servo.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::parseAddrMsg() {
		// Check checksum validity
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8] ^ _buffer[9] ^ _buffer[10] ^
		                   _buffer[11] ^ _buffer[12]) &
		                  0xFE;
		uint8_t chksum2 = (~_buffer[5] & 0xFE);

		if(chksum1 != _buffer[5] || chksum2 != _buffer[6]) {
			debug("Bad position Checksum\n\r");
		} else if(_callback_read_addr == nullptr) {
			debug("Callback for read addr is nullptr.\n\r");
		} else {
			// Check carried data length
			if(_buffer[8] == 1) {
				// Call the callback with status error, status detail, data[0] - id allready checked
				_callback_read_addr->call(_expected_reply_id, _buffer[10], _buffer[11], _buffer[9], 0);
			} else {
				// Call the callback with status error, status detail, data[0], data[1] - id allready checked
				_callback_read_addr->call(_expected_reply_id, _buffer[11], _buffer[12], _buffer[9], _buffer[10]);
			}

			// Reset the callback storage member
			_callback_read_addr = nullptr;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * parseStatMsg
	 * This function is an helper. It should not be called outside of cbInterpretBuffer.
	 * Parses a status ack message received from a servo.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::parseStatMsg() {
		// Check checksum validity
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8]) & 0xFE;
		uint8_t chksum2 = (~_buffer[5] & 0xFE);

		if(chksum1 != _buffer[5] || chksum2 != _buffer[6]) {
			debug("Bad status checksum.");
		} else if(_callback_read_stat == nullptr) {
			debug("Callback for read status is nullptr.");
		} else {
			// Call the callback with status error and status detail - id allready checked
			_callback_read_stat->call(_expected_reply_id, _buffer[7], _buffer[8]);

			// Reset the callback storage member
			_callback_read_stat = nullptr;
		}
	}
}

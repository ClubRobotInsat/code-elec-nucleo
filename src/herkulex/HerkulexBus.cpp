#include "HerkulexBus.h"

namespace herkulex {

	/* --------------------------------------------------------------------------------------------
	 * Constructeur (explicit)
	 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin.
	 * Log son activite sur le Serial pointe par log.
	 * Le baudrate de la com. peut etre specifie.
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::Bus(PinName txPin, PinName rxPin, uint32_t baudrate, float flush_frequency, DMAUsage usage)
	        : //	_ticker_flush()
	        /*,*/ _callback_waiting(false)
	        , _write_done(true)
	        , _ser(txPin, rxPin, baudrate)
	        , _read_callback()
	        , _write_callback(Callback<void(int)>(this, &Bus::cb_write_done))
	        , _buffer{}
	        , _buffer_write_data()
	        , _buffer_write_length()
	        , _total_write_length(0)
	        , _data_written(nullptr) {
		_ser.set_dma_usage_rx(usage);
		_ser.set_dma_usage_tx(usage);
		//_ticker_flush.attach(Callback<void()>(this, &Bus::flush_one_message), flush_frequency);
	}

	/* --------------------------------------------------------------------------------------------
	 * Destructeur
	 * Desalloue le Serial utilise par le bus
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::~Bus() {}

	void Bus::flush_one_message() {
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
			delete[] data;
			index += length;
		}
		_ser.write(message, _total_write_length, _write_callback, SERIAL_EVENT_TX_ALL);
		_write_done = false;
		_total_write_length = 0;
		_data_written = message;
	}

	void Bus::cb_write_done(int e) {
		_write_done = true;
		delete[] _data_written;
	}

	bool Bus::need_flush() {
		return _buffer_write_data.full();
	}

	/* --------------------------------------------------------------------------------------------
	 * sendMsg
	 * Construit un message pour les servos, et l'envoi immediatement sur le bus.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_msg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t* data, const uint8_t length) {
		uint8_t total_length = length + constants::Size::MinPacketSize;

		// NEW
		uint8_t* txBuf = new uint8_t[total_length];

		txBuf[0] = constants::header;
		txBuf[1] = constants::header;
		txBuf[2] = total_length;
		txBuf[3] = id;
		txBuf[4] = static_cast<uint8_t>(cmd);

		// Start construction of checksum
		txBuf[5] = txBuf[2] ^ txBuf[3] ^ txBuf[4];
		txBuf[6] = 0;

		if(data != nullptr) {
			for(uint8_t index = 0; index < length; ++index) {
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
	}

	void Bus::send_debug_message() {
		send_stat_msg(0xFD);
	}
	/* --------------------------------------------------------------------------------------------
	 * sendEEPWriteMsg
	 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_EEP_write_msg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendEEPWrite_msg avec une longueur erronee !\n");
		} else {
			uint8_t* data = new uint8_t[2 + len];

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			send_msg(id, constants::CMD::toServo::EEPWrite, data, (2 + len));
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendEEPReadMsg
	 * Construit un de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!!
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_EEP_read_msg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendEEPReadMsg avec une longueur erronee !\n");
		} else {
			uint8_t data[2];

			data[0] = addr;
			data[1] = len;

			send_msg(id, constants::CMD::toServo::EEPRead, data, 2);
		}
	}


	/* --------------------------------------------------------------------------------------------
	 * sendRAMWriteMsg
	 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_RAM_write_msg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} else {
			uint8_t* data = new uint8_t[2 + len];

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			send_msg(id, constants::CMD::toServo::RAMWrite, data, 2 + len);
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRAMReadMsg
	 * Construit un de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!!
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_RAM_read_msg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2) {
			debug("Utilisation de Bus::sendRAMWrite_msg avec une longueur erronee !\n");
		} else {
			uint8_t* data = new uint8_t[2 + len];

			data[0] = addr;
			data[1] = len;

			send_msg(id, constants::CMD::toServo::RAMRead, data, 2);
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendIJOGMsg
	 * Envoi d'une commande IJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex :
	 * -> champ SET d'un paquet IJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_IJOG_msg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		uint8_t* data = new uint8_t[5];

		data[0] = (jogValue & 0xff);
		data[1] = (jogValue & 0xff00) >> 8;
		data[2] = set;
		data[3] = id;
		data[4] = playtime;

		send_msg(id, constants::CMD::toServo::IJOG, data, 5);
	}

	/* --------------------------------------------------------------------------------------------
	 * sendSJOGMsg
	 * Envoi d'une commande SJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex :
	 * -> champ SET d'un paquet SJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_SJOG_msg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		uint8_t* data = new uint8_t[5];

		data[0] = playtime;
		data[1] = (jogValue & 0x00ff);
		data[2] = (jogValue & 0xff00) >> 8;
		data[3] = set;
		data[4] = id;

		send_msg(id, constants::CMD::toServo::SJOG, data, 5);
	}

	/* --------------------------------------------------------------------------------------------
	 * sendStatMsg
	 * Envoi un message de demande de status
	 !!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour
	 !!! la requete de status
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_stat_msg(uint8_t id) {
		uint8_t* data = new uint8_t[2];

		data[0] = 0;
		data[1] = 0;

		send_msg(id, constants::CMD::toServo::Stat, data, 2);
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRollbackMsg
	 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_rollback_msg(uint8_t id, bool skipIDRollback, bool skipBaudrateRollback) {
		uint8_t* data = new uint8_t[2];

		data[0] = (skipIDRollback ? 1 : 0);
		data[1] = (skipBaudrateRollback ? 1 : 0);

		send_msg(id, constants::CMD::toServo::Rollback, data, 2);
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRebootMsg
	 * Envoi un message de reboot
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::send_reboot_msg(uint8_t id) {
		send_msg(id, constants::CMD::toServo::Reboot);
	}
}

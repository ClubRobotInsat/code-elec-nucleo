#include "HerkulexBus.h"

namespace herkulex {

	/* --------------------------------------------------------------------------------------------
	 * Constructeur (explicit)
	 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin.
	 * Log son activite sur le Serial pointe par log.
	 * Le baudrate de la com. peut etre specifie.
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::Bus(PinName txPin, PinName rxPin, Serial* log, uint32_t baudrate, float flush_frequency)
	        : _ticker_flush()
	        , _callback_waiting(false)
	        , _write_done(true)
	        , _ser(txPin, rxPin, baudrate)
	        , _log(log)
	        , _read_callback(Callback<void(int)>(this, &Bus::cbInterpretBuffer))
	        , _write_callback(Callback<void(int)>(this, &Bus::cbWriteDone))
	        , _buffer_write_data()
	        , _buffer_write_length()
	        , _total_write_length(0) {
		_ticker_flush.attach(Callback<void()>(this, &Bus::flushOneMessage), flush_frequency);
	}

	/* --------------------------------------------------------------------------------------------
	 * Destructeur
	 * Desalloue le Serial utilise par le bus
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::~Bus() {
		_log->printf("Destruction du bus");
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
		;
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
		debug("Flushing\n\r");
		while(!_buffer_write_length.empty()) {
			uint8_t length;
			uint8_t* data;
			_buffer_write_length.pop(length);
			_buffer_write_data.pop(data);
			_log->printf("Sending : (%d) [", length);
			for(uint8_t i = 0; i < length; i++) {
				message[index + i] = data[i];
				_log->printf("%#x ", message[index + i]);
			}
			delete data;
			_log->printf("] in the next flush\n\r");
			index += length;
		}
		debug("Flushing : %d bytes \n\r", _total_write_length);
		_ser.write(message, _total_write_length, _write_callback, SERIAL_EVENT_TX_ALL);
		_write_done = false;
		_total_write_length = 0;
	}

	void Bus::cbWriteDone(int e) {
		debug("Write done\n\r");
		_write_done = true;
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
			debug("Waiting for previous write\n\r");
			wait_ms(1000);
		}

		_write_done = false;

		if(data == nullptr) {
			debug("Erreur nullptr data write\n\r");
		}
		debug("Write call\n\r");
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
				txBuf[5] ^= data[index];
			}
		}
		txBuf[5] = txBuf[5] & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

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
		debug("Callback \n\r");
		if(_buffer[3] == _expected_reply_id && _buffer[4] == _expected_reply_cmd) {
			switch(_buffer[4]) {
				// If we received an addr read ack
				case constants::CMD::fromServo::EEPReadAck:
					break;
				case constants::CMD::fromServo::RAMReadAck:
					debug("Received RAMREAD\n\r");
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
					_log->printf("Recu un message servo avec une mauvaise CMD\n");
					break;
			}
			_callback_waiting = false; // TODO - [supprimer], ou utiliser avec un timeout
		} else {
			debug("Bad ID");
		}
	}

	void Bus::sendDebugMessage() {
		_log->printf("Sending debug... \n\r");
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
}

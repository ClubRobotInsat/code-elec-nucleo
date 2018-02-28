#include "HerkulexBus.h"

namespace herkulex {

	/* --------------------------------------------------------------------------------------------
	 * Constructeur (explicit)
	 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin. 
	 * Log son activite sur le Serial pointe par log.
	 * Le baudrate de la com. peut etre specifie.  
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::Bus(PinName txPin, PinName rxPin, Serial * log, uint32_t baudrate) : 
		_callback_waiting(false), 
		_ser(txPin, rxPin, baudrate), 
		_log(log), 
		_read_callback(Callback<void(int)>(this, &Bus::cbInterpretBuffer)) {}

	/* --------------------------------------------------------------------------------------------
	 * Destructeur 
	 * Desalloue le Serial utilise par le bus
	 * --------------------------------------------------------------------------------------------
	 */
	Bus::~Bus() 
	{
		_log->printf("Destruction du bus");
	}

	/* --------------------------------------------------------------------------------------------
	 * write
	 * Prend un buffer (data), de taille fixe (length), passe par addresse et l'ecoule sur le bus 
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::write(uint8_t* data, uint8_t length) {
		//_log->printf("Writing\n");
		_log->printf("Sending : [");
		for(uint8_t i = 0; i < length; i++) {
			_ser.putc(data[i]);
			_log->printf("%#x ",data[i]);
		}
		_log->printf("]\n\r");

		//_ser.write(data, length, 0, 0);
		//_log->printf("Done writing\n");
	}

	/* --------------------------------------------------------------------------------------------
	 * sendMsg
	 * Construit un message pour les servos, et l'envoi immediatement sur le bus. 
	 * --------------------------------------------------------------------------------------------
	 */ 
	void Bus::sendMsg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t * data, const uint8_t length)
	{
		uint8_t total_length = length + constants::Size::MinPacketSize;
		uint8_t index = 0;

		// NEW 
		uint8_t * txBuf = new uint8_t(total_length); 

		txBuf[0] = constants::header;
		txBuf[1] = constants::header;
		txBuf[2] = total_length;
		txBuf[3] = id;
		txBuf[4] = cmd;

		// Start construction of checksum
		txBuf[5] = txBuf[2] ^ txBuf[3] ^ txBuf[4];
		txBuf[6] = 0;

		if(data != nullptr)
		{
			for(index = 0; index < length; ++index) {
				txBuf[constants::Size::MinPacketSize + index] = data[index];
				// Iteratively construct the checksum
				txBuf[5] ^= txBuf[index];
			}
		}

		txBuf[6] = (~txBuf[5]) & 0xFE;

		write(txBuf, total_length);

		// DELETE
		delete txBuf;
	}


	/* --------------------------------------------------------------------------------------------
	 * cbInterpretBuffer
	 * Callback pour lire et interpreter un message recu. Globalement, switch sur le type de 
	 * message et appelle le parser correspondant au message.
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::cbInterpretBuffer(int event) {
		if( _buffer[3] == _expected_reply_id && _buffer[4] == _expected_reply_cmd) 
		{
			switch(_buffer[4]) 
			{
				// If we received an addr read ack
				case constants::CMD::fromServo::EEPReadAck:
				case constants::CMD::fromServo::RAMReadAck:
					parseAddrMsg();
					break; 

				// If we received a status read ack
				case constants::CMD::fromServo::StatAck:
					parseStatMsg();
					break; 

				// Do nothing for other replies
				case constants::CMD::fromServo::EEPWriteAck:
				case constants::CMD::fromServo::RAMWriteAck:
				case constants::CMD::fromServo::IJOGAck:
				case constants::CMD::fromServo::SJOGAck:
				case constants::CMD::fromServo::RollbackAck:
				case constants::CMD::fromServo::RebootAck:
					break; 

				default: 
					_log->printf("Recu un message servo avec une mauvaise CMD\n");
					break; 
			}
			_callback_waiting = false; // TODO - [supprimer], ou utiliser avec un timeout
		} 
		else 
		{
			debug("Bad ID");
		}
	}
}

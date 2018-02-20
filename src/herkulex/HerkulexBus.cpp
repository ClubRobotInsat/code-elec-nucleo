#include "HerkulexBus.h"

namespace herkulex {
	Bus::Bus(PinName txPin, PinName rxPin, Serial* log)
	        : _callback_waiting(false)
	        , _ser(new Serial(txPin, rxPin, 115200))
	        , _log(log)
	        , _read_callback(Callback<void(int)>(this, &Bus::interpretBuffer)) {}

	// Servo Bus::makeNewServo(uint8_t id) {
	// 	return Servo(id, this);
	// }

	void Bus::write(uint8_t* data, uint8_t length) {
		//_log->printf("Writing\n");
		for(uint8_t i = 0; i < length; i++) {
			_ser->putc(data[i]);
		}

		//_ser->write(data, length, 0, 0);
		//_log->printf("Done writing\n");
	}

	void Bus::interpretBuffer(int event) {

		if( (!_buffer[3]) != _servo_registered_for_callback->getId()) {

			if(_buffer[4] == 0X47) {
				this->parseStatusMessage(_servo_registered_for_callback);
			} else if(_buffer[4] == 0x46) {
				this->parsePositionMessage(_servo_registered_for_callback);
			} else {
				debug("Bad CMD");
			}
			_callback_waiting = false;
		} else {
			debug("Bad ID");
		}
	}

	void Bus::parseStatusMessage(Servo* servo) {
		// Checksum1
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8]) & 0xFE;
		if(chksum1 != _buffer[5]) {
			debug("Bad status Checksum #1");
			servo->setStatus(0xFF);
		}

		// Checksum2
		uint8_t chksum2 = (~_buffer[5] & 0xFE);
		if(chksum2 != _buffer[6]) {
			debug("Bad status Checksum #2");
			servo->setStatus(0xFF);
		}

		servo->setStatus(_buffer[7]); // Status Error
		                              // status = _buffer[8];  // Status Detail
	}

	void Bus::parsePositionMessage(Servo* servo) {
		// Checksum1
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8] ^ _buffer[9] ^ _buffer[10] ^
		                   _buffer[11] ^ _buffer[12]) &
		                  0xFE;
		if(chksum1 != _buffer[5]) {
			debug("Bad position Checksum #1");
			servo->setPosition(0xFFFF);
		}

		// Checksum2
		uint8_t chksum2 = (~_buffer[5] & 0xFE);
		if(chksum2 != _buffer[6]) {
			debug("Bad position Checksum #2");
			servo->setPosition(0xFFFF);
		}

		servo->setPosition(((_buffer[10] & 0x03) << 8) | _buffer[9]);
	}

	void Bus::fetchStatus(Servo* servo) {
		uint8_t txBuf[7];

		txBuf[0] = constants::header;          // Packet Header (0xFF)
		txBuf[1] = constants::header;          // Packet Header (0xFF)
		txBuf[2] = constants::Size::MinPacketSize; // Packet Size
		txBuf[3] = servo->getId();  // Servo ID
		txBuf[4] = constants::CMD::toServo::Stat;        // Status Error, Status Detail request

		// Check Sum1 and Check Sum2
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

		// Wait for a free slot.
		while(_callback_waiting) {
		}
		_callback_waiting = true;
		_servo_registered_for_callback = servo;

		// Bus::send packet (mbed -> herkulex)
		this->write(txBuf, 7);

		_ser->read(_buffer, (uint8_t)9, _read_callback, SERIAL_EVENT_RX_COMPLETE);
	}

	void Bus::fetchPosition(Servo* servo) {
		uint8_t txBuf[9];

		txBuf[0] = static_cast<uint8_t>(constants::header);                  // Packet Header (0xFF)
		txBuf[1] = static_cast<uint8_t>(constants::header);                  // Packet Header (0xFF)
		txBuf[2] = static_cast<uint8_t>(constants::Size::MinPacketSize) + 2;     // Packet Size
		txBuf[3] = servo->getId();          // Servo ID
		txBuf[4] = static_cast<uint8_t>(constants::CMD::toServo::RAMRead);            // Status Error, Status Detail request
		txBuf[5] = 0;                       // Checksum1
		txBuf[6] = 0;                       // Checksum2
		txBuf[7] = static_cast<uint8_t>(constants::RAMAddr::CalibratedPosition); // Address 58
		txBuf[8] = 2;                   // Address 58 and 59

		// Check Sum1 and Check Sum2
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

		// Wait that there is a free slot for us.
		while(_callback_waiting) {
		}
		_callback_waiting = true;
		_servo_registered_for_callback = servo;

		// Bus::send packet (mbed -> herkulex)
		this->write(txBuf, 9);

		_ser->read(_buffer, (uint8_t)13, _read_callback, SERIAL_EVENT_RX_COMPLETE);
	}


	void Bus::sendMsg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t* data, const uint8_t length)
	{
		uint8_t total_length = length + constants::Size::MinPacketSize;
		uint8_t * txBuf = new uint8_t(total_length);
		uint8_t index = 0;

		txBuf[0] = constants::header;
		txBuf[1] = constants::header;
		txBuf[2] = total_length;
		txBuf[3] = id;
		txBuf[4] = cmd;

		// Start construction of checksum
		txBuf[5] = txBuf[2] ^ txBuf[3] ^ txBuf[4];
		txBuf[6] = 0;

		for(index = 0; index < length; ++index) {
			txBuf[static_cast<uint8_t>(constants::Size::MinPacketSize) + index] = data[index];
			// Iteratively construct the checksum
			txBuf[5] ^= txBuf[index];
		}

		txBuf[6] = (~txBuf[5]) & 0xFE;

		write(txBuf, total_length);
		delete txBuf;
	}

	void Bus::sendEEPWriteMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendEEPWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			uint8_t* data = new uint8_t(2 + len);
	
			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}
	
			sendMsg(id, constants::CMD::toServo::EEPWrite, data, (2 + len));
		}
	}

	void Bus::sendEEPReadMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendEEPReadMsg avec une longueur erronee !\n");
		} 
		else 
		{
			uint8_t data[2];

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::EEPRead, data, 2);
		}
	}

	void Bus::sendRAMWriteMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			uint8_t* data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			sendMsg(id, constants::CMD::toServo::RAMWrite, data, 2 + len);
		}
	}

	void Bus::sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			uint8_t* data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::RAMRead, data, 2);
		}
	}

	Bus::~Bus() 
	{
		_log->printf("Destruction du bus");
	}
	// void Bus::sendIJOGMsg();
	// void Bus::sendSJOGMsg();
	// void Bus::sendStatMsg();
	// void Bus::sendRollbackMsg();
	// void Bus::sendRebootMsg();
}

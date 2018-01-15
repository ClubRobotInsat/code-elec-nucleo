#include "bus.h"

HerkulexBus::HerkulexBus(Serial* ser)
        : _callback_waiting(false), _ser(ser), _read_callback(Callback<void(int)>(this, &HerkulexBus::interpretBuffer)) {}

HerkulexServo HerkulexBus::makeNewServo(uint8_t id) {
	return HerkulexServo(id, this);
}

void HerkulexBus::write(uint8_t* data, uint8_t length) {
	_ser->write(data, length, 0, 0);
}

void HerkulexBus::interpretBuffer(int event) {
	if(_buffer[4] == 0X47) {
		this->parseStatusMessage(_servo_registered_for_callback);
	} else if(_buffer[4] == 0x46) {
		this->parsePositionMessage(_servo_registered_for_callback);
	} else {
		/// FIXME : Erreur
	}
	_callback_waiting = false;
}

void HerkulexBus::parseStatusMessage(HerkulexServo* servo) {
	// Checksum1
	uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8]) & 0xFE;
	if(chksum1 != _buffer[5]) {
		servo->setStatus(0xFF);
	}

	// Checksum2
	uint8_t chksum2 = (~_buffer[5] & 0xFE);
	if(chksum2 != _buffer[6]) {
		servo->setStatus(0xFF);
	}

	servo->setStatus(_buffer[7]); // Status Error
	                              // status = _buffer[8];  // Status Detail
}

void HerkulexBus::parsePositionMessage(HerkulexServo* servo) {
	// Checksum1
	uint8_t chksum1 =
	    (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8] ^ _buffer[9] ^ _buffer[10] ^ _buffer[11] ^ _buffer[12]) & 0xFE;
	if(chksum1 != _buffer[5]) {
		servo->setPosition(0xFFFF);
	}

	// Checksum2
	uint8_t chksum2 = (~_buffer[5] & 0xFE);
	if(chksum2 != _buffer[6]) {
		servo->setPosition(0xFFFF);
	}

	servo->setPosition(((_buffer[10] & 0x03) << 8) | _buffer[9]);
}

void HerkulexBus::fetchStatus(HerkulexServo* servo) {
	uint8_t txBuf[7];

	txBuf[0] = HEADER;          // Packet Header (0xFF)
	txBuf[1] = HEADER;          // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE; // Packet Size
	txBuf[3] = servo->getId();  // Servo ID
	txBuf[4] = CMD_STAT;        // Status Error, Status Detail request

	// Check Sum1 and Check Sum2
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// Wait for a free slot.
	while(_callback_waiting) {
	}
	_callback_waiting = true;
	_servo_registered_for_callback = servo;

	// send packet (mbed -> herkulex)
	this->write(txBuf, 7);

	_ser->read(_buffer, (uint8_t)9, _read_callback, SERIAL_EVENT_RX_COMPLETE);
}

void HerkulexBus::fetchPosition(HerkulexServo* servo) {
	uint8_t txBuf[9];

	txBuf[0] = HEADER;                  // Packet Header (0xFF)
	txBuf[1] = HEADER;                  // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 2;     // Packet Size
	txBuf[3] = servo->getId();          // Servo ID
	txBuf[4] = CMD_RAM_READ;            // Status Error, Status Detail request
	txBuf[5] = 0;                       // Checksum1
	txBuf[6] = 0;                       // Checksum2
	txBuf[7] = RAM_CALIBRATED_POSITION; // Address 52
	txBuf[8] = BYTE2;                   // Address 52 and 53

	// Check Sum1 and Check Sum2
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// Wait that there is a free slot for us.
	while(_callback_waiting) {
	}
	_callback_waiting = true;
	_servo_registered_for_callback = servo;

	// send packet (mbed -> herkulex)
	this->write(txBuf, 9);

	_ser->read(_buffer, (uint8_t)13, _read_callback, SERIAL_EVENT_RX_COMPLETE);
}

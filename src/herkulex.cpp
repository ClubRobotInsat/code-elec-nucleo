//------------------------------------------------------------------------------
/* herkulex servo library for mbed
 *
 * Copyright (c) 2012-2013 Yoonseok Pyo, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
//------------------------------------------------------------------------------
#include "herkulex.h"
#include "mbed.h"

void Herkulex::interpretBuffer(int event) {

	if(_buffer[4] == 0X47) {
		this->parseStatusMessage();
	}
	// FIXME : Mettre le bon ID.
	else if(_buffer[4] == 0x46) {
		this->parsePositionMessage();
	} else {
		_pc->printf("Erreur de commande");
	}
}

void Herkulex::parseStatusMessage() {
	// Checksum1
	uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8]) & 0xFE;
	if(chksum1 != _buffer[5]) {
#ifdef HERKULEX_DEBUG
		_pc->printf("Checksum1 fault\n");
#endif

		_status = -1;
	}

	// Checksum2
	uint8_t chksum2 = (~_buffer[5] & 0xFE);
	if(chksum2 != _buffer[6]) {
#ifdef HERKULEX_DEBUG
		_pc->printf("Checksum2 fault\n");
#endif

		_status = -1;
	}

	_status = _buffer[7]; // Status Error
                          // status = _buffer[8];  // Status Detail
#ifdef HERKULEX_DEBUG
	_pc->printf("Status = %02X\n", _status);
#endif
}

Herkulex::Herkulex(uint8_t id, Serial* connection, Serial* pc)
        : _status_position(0)
        , _status(0)
        , _id(id)
        , _callback_read(Callback<void(int)>(this, &Herkulex::interpretBuffer))
        , _pc(pc)
        , _ser(connection) {
	_pc->printf("OK\n");
	this->fetchStatus();
}

void Herkulex::parsePositionMessage() {
	// Checksum1
	uint8_t chksum1 =
	    (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8] ^ _buffer[9] ^ _buffer[10] ^ _buffer[11] ^ _buffer[12]) & 0xFE;
	if(chksum1 != _buffer[5]) {
#ifdef HERKULEX_DEBUG
		_pc->printf("Checksum1 fault\n");
#endif

		_status_position = -1;
	}

	// Checksum2
	uint8_t chksum2 = (~_buffer[5] & 0xFE);
	if(chksum2 != _buffer[6]) {
#ifdef HERKULEX_DEBUG
		_pc->printf("Checksum2 fault\n");
#endif
		_status_position = -1;
	}

	_status_position = ((_buffer[10] & 0x03) << 8) | _buffer[9];

#ifdef HERKULEX_DEBUG
	_pc->printf("position = %04X(%d)\n", _status_position, _status_position);
#endif
}

//------------------------------------------------------------------------------
Herkulex::~Herkulex() {
	if(_ser != NULL)
		delete _ser;
}

//------------------------------------------------------------------------------
void Herkulex::txPacket(uint8_t packetSize, uint8_t* data) {
#ifdef HERKULEX_DEBUG
	_pc->printf("[TX]");
#endif

	for(uint8_t i = 0; i < packetSize; i++) {
		_ser->putc(data[i]);
	}
//_ser->write(data, packetSize, 0, 0);

#ifdef HERKULEX_DEBUG
	_pc->printf("\n");
#endif
}
//------------------------------------------------------------------------------
void Herkulex::clear() {
	uint8_t txBuf[11];

	txBuf[0] = HEADER;              // Packet Header (0xFF)
	txBuf[1] = HEADER;              // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 4; // Packet Size
	txBuf[3] = _id;                 // Servo ID
	txBuf[4] = CMD_RAM_WRITE;       // Command Ram Write (0x03)
	txBuf[5] = 0;                   // Checksum1
	txBuf[6] = 0;                   // Checksum2
	txBuf[7] = RAM_STATUS_ERROR;    // Address 48
	txBuf[8] = BYTE2;               // Length
	txBuf[9] = 0;                   // Clear RAM_STATUS_ERROR
	txBuf[10] = 0;                  // Clear RAM_STATUS_DETAIL

	// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
	// Checksum2 = (~Checksum1)&0xFE
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(11, txBuf);
}

//------------------------------------------------------------------------------
void Herkulex::setTorque(uint8_t cmdTorue) {
	uint8_t txBuf[10];

	txBuf[0] = HEADER;              // Packet Header (0xFF)
	txBuf[1] = HEADER;              // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 3; // Packet Size
	txBuf[3] = _id;                 // Servo ID
	txBuf[4] = CMD_RAM_WRITE;       // Command Ram Write (0x03)
	txBuf[5] = 0;                   // Checksum1
	txBuf[6] = 0;                   // Checksum2
	txBuf[7] = RAM_TORQUE_CONTROL;  // Address 52
	txBuf[8] = BYTE1;               // Length
	txBuf[9] = cmdTorue;            // Torque ON

	// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
	// Checksum2 = (~Checksum1)&0xFE
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(10, txBuf);
}

//------------------------------------------------------------------------------
void Herkulex::positionControl(uint16_t position, uint8_t playtime, uint8_t setLED) {
	if(position > 1023)
		return;

	uint8_t txBuf[12];

	txBuf[0] = HEADER;                   // Packet Header (0xFF)
	txBuf[1] = HEADER;                   // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 5;      // Packet Size
	txBuf[3] = _id;                      // pID is the id of the servo (0 ~ 253)
	txBuf[4] = CMD_S_JOG;                // Command S JOG (0x06)
	txBuf[5] = 0;                        // Checksum1
	txBuf[6] = 0;                        // Checksum2
	txBuf[7] = playtime;                 // Playtime
	txBuf[8] = position & 0x00FF;        // Position (LSB, Least Significant Bit)
	txBuf[9] = (position & 0xFF00) >> 8; // position (MSB, Most Significanct Bit)
	txBuf[10] = POS_MODE | setLED;       // Pos Mode and LED on/off
	txBuf[11] = _id;                     // Servo ID

	// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
	// Checksum2 = (~Checksum1)&0xFE
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10] ^ txBuf[11]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(12, txBuf);
}

//------------------------------------------------------------------------------
void Herkulex::velocityControl(int16_t speed, uint8_t setLED) {
	if(speed > 1023 || speed < -1023)
		return;

	uint8_t txBuf[12];

	txBuf[0] = HEADER;                // Packet Header (0xFF)
	txBuf[1] = HEADER;                // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 5;   // Packet Size
	txBuf[3] = _id;                   // pID is total number of servos in the network (0 ~ 253)
	txBuf[4] = CMD_S_JOG;             // Command S JOG (0x06)
	txBuf[5] = 0;                     // Checksum1
	txBuf[6] = 0;                     // Checksum2
	txBuf[7] = 0;                     // Playtime, unmeaningful in turn mode
	txBuf[8] = speed & 0x00FF;        // Speed (LSB, Least Significant Bit)
	txBuf[9] = (speed & 0xFF00) >> 8; // Speed (MSB, Most Significanct Bit)
	txBuf[10] = TURN_MODE | setLED;   // Turn Mode and LED on/off
	txBuf[11] = _id;                  // Servo ID

	// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
	// Checksum2 = (~Checksum1)&0xFE
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10] ^ txBuf[11]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(12, txBuf);
}


void Herkulex::setLedOn() {
	uint8_t txBuf[10];

	txBuf[0] = HEADER;              // Packet Header (0xFF)
	txBuf[1] = HEADER;              // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 3; // Packet Size
	txBuf[3] = _id;                 // pID is total number of servos in the network (0 ~ 253)
	txBuf[4] = CMD_RAM_WRITE;       // Command S JOG (CMD_RAM_WRITE)
	txBuf[5] = 0;                   // Checksum1
	txBuf[6] = 0;                   // Checksum2
	txBuf[7] = 0x35;                // Playtime, unmeaningful in turn mode
	txBuf[8] = 0x01;                // Speed (LSB, Least Significant Bit)
	txBuf[9] = 0xff;                // Speed (MSB, Most Significanct Bit)
	// txBuf[10] = TURN_MODE | setLED;  // Turn Mode and LED on/off

	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	txPacket(10, txBuf);
}

//------------------------------------------------------------------------------
int8_t Herkulex::getStatus() {
	return _status;
}

//------------------------------------------------------------------------------
int16_t Herkulex::getPos() {
	return _status_position;
}

void Herkulex::fetchStatus() {
	uint8_t txBuf[7];

	txBuf[0] = HEADER;          // Packet Header (0xFF)
	txBuf[1] = HEADER;          // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE; // Packet Size
	txBuf[3] = _id;             // Servo ID
	txBuf[4] = CMD_STAT;        // Status Error, Status Detail request

	// Check Sum1 and Check Sum2
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(7, txBuf);

	_ser->read(_buffer, (uint8_t)9, _callback_read, SERIAL_EVENT_RX_COMPLETE);
}

void Herkulex::fetchPosition() {
	uint8_t txBuf[9];

	txBuf[0] = HEADER;                  // Packet Header (0xFF)
	txBuf[1] = HEADER;                  // Packet Header (0xFF)
	txBuf[2] = MIN_PACKET_SIZE + 2;     // Packet Size
	txBuf[3] = _id;                     // Servo ID
	txBuf[4] = CMD_RAM_READ;            // Status Error, Status Detail request
	txBuf[5] = 0;                       // Checksum1
	txBuf[6] = 0;                       // Checksum2
	txBuf[7] = RAM_CALIBRATED_POSITION; // Address 52
	txBuf[8] = BYTE2;                   // Address 52 and 53

	// Check Sum1 and Check Sum2
	txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8]) & 0xFE;
	txBuf[6] = (~txBuf[5]) & 0xFE;

	// send packet (mbed -> herkulex)
	txPacket(9, txBuf);

	_ser->read(_buffer, (uint8_t)13, _callback_read, SERIAL_EVENT_RX_COMPLETE);
}

//------------------------------------------------------------------------------

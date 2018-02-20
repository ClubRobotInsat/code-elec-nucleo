#include "HerkulexServo.h"
#include "HerkulexBus.h"

namespace herkulex {
	Servo::Servo(uint8_t id, Bus& bus, Serial* log) : _id(id), _status(0), _position(0), _bus(bus), _log(log) {
		_log->printf("Hello from servo \n");
	}

	uint16_t Servo::getPosition() const {
		return _position;
	}

	uint8_t Servo::getStatus() const {
		return _status;
	}

	uint8_t Servo::getId() const {
		return _id;
	}

	void Servo::setStatus(uint8_t newStatus) {
		_status = newStatus;
	}

	void Servo::setPosition(uint16_t newPosition) {
		_position = newPosition;
	}

	void Servo::clear() const {
		//_log->printf("slt\n");
		uint8_t txBuf[11];

		// txBuf[0] = HEADER;              // Packet Header (0xFF)
		// txBuf[1] = HEADER;              // Packet Header (0xFF)
		// txBuf[2] = MIN_PACKET_SIZE + 4; // Packet Size
		// txBuf[3] = _id;                 // Servo ID
		// txBuf[4] = CMD_RAM_WRITE;       // Command Ram Write (0x03)
		// txBuf[5] = 0;                   // Checksum1
		// txBuf[6] = 0;                   // Checksum2
		// txBuf[7] = RAM_STATUS_ERROR;    // Address 48
		// txBuf[8] = BYTE2;               // Length
		// txBuf[9] = 0;                   // Clear RAM_STATUS_ERROR
		// txBuf[10] = 0;                  // Clear RAM_STATUS_DETAIL

		// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
		// Checksum2 = (~Checksum1)&0xFE
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;
		//_log->printf("Ici aussi \n");
		// send packet (mbed -> herkulex)
		// _bus.write(txBuf, 11);
	}

	void Servo::setTorque(uint8_t torque) const {
		uint8_t txBuf[10];

		// txBuf[0] = HEADER;              // Packet Header (0xFF)
		// txBuf[1] = HEADER;              // Packet Header (0xFF)
		// txBuf[2] = MIN_PACKET_SIZE + 3; // Packet Size
		// txBuf[3] = _id;                 // Servo ID
		// txBuf[4] = CMD_RAM_WRITE;       // Command Ram Write (0x03)
		// txBuf[5] = 0;                   // Checksum1
		// txBuf[6] = 0;                   // Checksum2
		// txBuf[7] = RAM_TORQUE_CONTROL;  // Address 52
		// txBuf[8] = BYTE1;               // Length
		// txBuf[9] = torque;              // Torque ON

		// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
		// Checksum2 = (~Checksum1)&0xFE
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;
		_log->printf("Coucou \n");
		// send packet (mbed -> herkulex)
		// _bus.write(txBuf, 10);
	}

	//------------------------------------------------------------------------------
	void Servo::positionControl(uint16_t position, uint8_t playtime, uint8_t setLED) const {
		if(position > 1023)
			return;

		uint8_t txBuf[12];

		// txBuf[0] = HEADER;                   // Packet Header (0xFF)
		// txBuf[1] = HEADER;                   // Packet Header (0xFF)
		// txBuf[2] = MIN_PACKET_SIZE + 5;      // Packet Size
		// txBuf[3] = _id;                      // pID is the id of the servo (0 ~ 253)
		// txBuf[4] = CMD_S_JOG;                // Command S JOG (0x06)
		// txBuf[5] = 0;                        // Checksum1
		// txBuf[6] = 0;                        // Checksum2
		// txBuf[7] = playtime;                 // Playtime
		// txBuf[8] = position & 0x00FF;        // Position (LSB, Least Significant Bit)
		// txBuf[9] = (position & 0xFF00) >> 8; // position (MSB, Most Significanct Bit)
		// txBuf[10] = POS_MODE | setLED;       // Pos Mode and LED on/off
		// txBuf[11] = _id;                     // Servo ID

		// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
		// Checksum2 = (~Checksum1)&0xFE
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10] ^ txBuf[11]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

		// send packet (mbed -> herkulex)
		// _bus.write(txBuf, 12);
	}

	//------------------------------------------------------------------------------
	void Servo::velocityControl(int16_t speed, uint8_t setLED) const {
		if(speed > 1023 || speed < -1023)
			return;

		uint8_t txBuf[12];

		// txBuf[0] = HEADER;                // Packet Header (0xFF)
		// txBuf[1] = HEADER;                // Packet Header (0xFF)
		// txBuf[2] = MIN_PACKET_SIZE + 5;   // Packet Size
		// txBuf[3] = _id;                   // pID is total number of servos in the network (0 ~ 253)
		// txBuf[4] = CMD_S_JOG;             // Command S JOG (0x06)
		// txBuf[5] = 0;                     // Checksum1
		// txBuf[6] = 0;                     // Checksum2
		// txBuf[7] = 0;                     // Playtime, unmeaningful in turn mode
		// txBuf[8] = speed & 0x00FF;        // Speed (LSB, Least Significant Bit)
		// txBuf[9] = (speed & 0xFF00) >> 8; // Speed (MSB, Most Significanct Bit)
		// txBuf[10] = TURN_MODE | setLED;   // Turn Mode and LED on/off
		// txBuf[11] = _id;                  // Servo ID

		// Checksum1 = (PacketSize ^ pID ^ CMD ^ Data[0] ^ Data[1] ^ ... ^ Data[n]) & 0xFE
		// Checksum2 = (~Checksum1)&0xFE
		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9] ^ txBuf[10] ^ txBuf[11]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

		// send packet (mbed -> herkulex)
		// _bus.write(txBuf, 12);
	}


	void Servo::setLedOn() const {
		uint8_t txBuf[10];

		// txBuf[0] = HEADER;              // Packet Header (0xFF)
		// txBuf[1] = HEADER;              // Packet Header (0xFF)
		// txBuf[2] = MIN_PACKET_SIZE + 3; // Packet Size
		// txBuf[3] = _id;                 // pID is total number of servos in the network (0 ~ 253)
		// txBuf[4] = CMD_RAM_WRITE;       // Command S JOG (CMD_RAM_WRITE)
		// txBuf[5] = 0;                   // Checksum1
		// txBuf[6] = 0;                   // Checksum2
		// txBuf[7] = 0x35;                // Playtime, unmeaningful in turn mode
		// txBuf[8] = 0x01;                // Speed (LSB, Least Significant Bit)
		// txBuf[9] = 0xff;                // Speed (MSB, Most Significanct Bit)
		// txBuf[10] = TURN_MODE | setLED;  // Turn Mode and LED on/off

		txBuf[5] = (txBuf[2] ^ txBuf[3] ^ txBuf[4] ^ txBuf[7] ^ txBuf[8] ^ txBuf[9]) & 0xFE;
		txBuf[6] = (~txBuf[5]) & 0xFE;

		// _bus.write(txBuf, 10);
	}

	void Servo::updatePosition() {
		// _bus.fetchPosition(this);
	}

	void Servo::updateStatus() {
		// _bus.fetchStatus(this);
	}

	void Servo::setId(uint8_t id) {
		_id = id;
	}
}

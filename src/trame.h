#ifndef TRAME_H
#define TRAME_H

#include "mbed.h"

#define BITS_CMD_TRAME 4
#define BITS_ID_TRAME 7

class Trame {

public:
	Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data);
	Trame();

	static uint8_t* makeAck(uint8_t num_packet);

	uint8_t* getData() const {
		return _data;
	}
	uint8_t getId() const {
		return _id;
	}
	uint8_t getDataLength() const {
		return _data_length;
	}
	uint8_t getCmd() const {
		return _cmd;
	}

	// Envoie la trame sur la connexion série.
	void send(Serial* pc) const;

	static uint8_t demultiplexId(uint8_t const& first, uint8_t const& second) {
		uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
		return uint8_t((muxedVal >> BITS_CMD_TRAME) & ~(1 << BITS_ID_TRAME));
	}

	static uint8_t demultiplexCmd(uint8_t const& first, uint8_t const& second) {
		uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
		return uint8_t(muxedVal & 0xF);
	}


	// ajoute une donnée à la trame
	void appendData(uint8_t data);

private:
	uint8_t _id, _cmd, _data_length;
	uint8_t* _data;
};

#endif

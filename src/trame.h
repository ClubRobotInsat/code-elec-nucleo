#ifndef TRAME_H
#define TRAME_H

#include "mbed.h"

#define BITS_CMD_TRAME 4
#define BITS_ID_TRAME 7
#define HEADER_SIZE 4

class Trame {

public:
	Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number);
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
	uint8_t getPacketNumber() const {
		return _packet_number;
	}

	// Envoie la trame sur la connexion série.
	void sendToCanAck(Serial* pc);

	void sendToCan(Serial* pc);

	// Envoie la trame a la carte CAN-USB
	static uint8_t demultiplexId(uint8_t const& first, uint8_t const& second);

	static uint8_t demultiplexCmd(uint8_t const& first, uint8_t const& second);

	static uint8_t multiplexId(uint8_t id, uint8_t cmd);

	static uint8_t multiplexCmd(uint8_t id, uint8_t cmd);

	// ajoute une donnée à la trame
	void appendData(uint8_t data);

private:
	uint8_t _id, _cmd, _data_length, _packet_number;
	uint8_t* _data;
	event_callback_t _write_callback;
	uint8_t* _data_to_delete;

	void deleteDataWrite(int event);
};

#endif

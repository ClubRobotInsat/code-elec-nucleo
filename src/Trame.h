#ifndef TRAME_H
#define TRAME_H

#include "Buffer.h"
#include <mbed.h>

#define BITS_CMD_TRAME 4
#define BITS_ID_TRAME 7
#define HEADER_SIZE 4
#define TRAME_DATA_SIZE 16

class Trame {

public:
	Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number);

	Trame();

	~Trame() = default;

	explicit Trame(CANMessage);

	void print() const {
		debug("ID : %#x | CMD : %#x | N° : %#x | DL : %#x \n\r", _id, _cmd, _packet_number, _data_length);
	}

	uint8_t* get_data() {
		return _data;
	}

	const uint8_t* get_data() const {
		return _data;
	}

	uint8_t get_id() const {
		return _id;
	}
	uint8_t get_data_length() const {
		return _data_length;
	}
	uint8_t get_cmd() const {
		return _cmd;
	}
	uint8_t get_packet_number() const {
		return _packet_number;
	}

	// Envoie une trame d'acquitement sur la connexion série.
	static void send_ack(uint8_t packet_number, Serial* pc);

	// Envoie un pong pour l'ID `id`.
	static void send_pong(uint8_t id, Serial* pc);

	// Envoie la trame sur la connexion série
	void send_to_serial();

	CANMessage into_can_message();

	// Renvoie vrai si la trame est un ping.
	bool is_ping() const;

	/* Fonctions utilitaires permettant de sérialiser et déserialiser des trames facilement */

	static uint8_t demultiplex_id(uint8_t const first, uint8_t const second);

	static uint8_t demultiplex_cmd(uint8_t const first, uint8_t const second);

	static uint8_t multiplex_id(uint8_t const id, uint8_t const cmd);

	static uint8_t multiplex_cmd(uint8_t const id, uint8_t const cmd);

	static uint8_t demultiplex_id(uint16_t const);

	static uint8_t demultiplex_cmd(uint16_t const);

	static uint16_t multiplex_id_and_cmd(uint8_t const id, uint8_t const cmd);

private:
	static uint8_t* make_ack(uint8_t num_packet);
	uint8_t _id, _cmd, _data_length, _packet_number;
	uint8_t _data[TRAME_DATA_SIZE];
};

#endif

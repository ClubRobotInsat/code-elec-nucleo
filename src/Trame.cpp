#include "Trame.h"

Trame::Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number)
        : _id(id), _cmd(cmd), _data_length(data_length), _packet_number(packet_number), _data{0} {
	if(data_length < 16) {
		memcpy(_data, data, data_length);
	} else {
		error("Invalid trame data length \n\r");
	}
}

Trame::Trame() : _id(0), _cmd(0), _data_length(0), _packet_number(0), _data{0} {}

void Trame::send_ack(uint8_t packet_number, Serial* pc) {
	int size = 8;
	uint8_t* tab = new uint8_t[size];

	// Remplir le tableau contenant la trame
	tab[0] = 0xAC;
	tab[1] = 0xDC;
	tab[2] = 0xAB;
	tab[3] = 0xBB;
	tab[4] = packet_number;
	tab[5] = 0;
	tab[6] = 0;
	tab[7] = 0;

	// printf("Envoi du ack : ");
	for(uint8_t k; k < 8; k++) {
		// printf("%#x ",tab[k]);
	}
	// printf("\n\r");

	Buffer* buffer = new Buffer(tab, size);
	buffer->write(pc);
}


void Trame::send_to_can(Serial* pc) {
	int size = 8 + _data_length;
	uint8_t* tab = new uint8_t[size];

	// Remplir le tableau contenant la trame
	tab[0] = 0xAC;
	tab[1] = 0xDC;
	tab[2] = 0xAB;
	tab[3] = 0xBA;
	tab[4] = _packet_number; // FIXME
	tab[5] = Trame::multiplex_id(_id, _cmd);
	tab[6] = Trame::multiplex_cmd(_id, _cmd);
	tab[7] = _data_length;
	for(int j = 0; j < _data_length; j++) {
		tab[8 + j] = _data[j];
	}
	Buffer* buffer = new Buffer(tab, size);
	buffer->write(pc);
}

void Trame::send_pong(uint8_t id, Serial* pc) {

	uint8_t* tab = new uint8_t[8];

	tab[0] = 0xAC;
	tab[1] = 0xDC;
	tab[2] = 0xAB;
	tab[3] = 0xBA;
	tab[4] = Trame::multiplex_id(id, 0x00);
	tab[5] = Trame::multiplex_cmd(id, 0x00);
	tab[6] = 0x01;
	tab[7] = 0xAA;

	Buffer* buffer = new Buffer(tab, 8);
	buffer->write(pc);
}

bool Trame::is_ping() const {
	return _data_length == 1 && _cmd == 0x00 && _data[0] == 0x55;
}

uint8_t Trame::demultiplex_id(uint8_t const& first, uint8_t const& second) {
	uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
	return uint8_t((muxedVal >> BITS_CMD_TRAME) & ~(1 << BITS_ID_TRAME));
}

uint8_t Trame::demultiplex_cmd(uint8_t const& first, uint8_t const& second) {
	uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
	return uint8_t(muxedVal & 0xF);
}

uint8_t Trame::multiplex_id(uint8_t id, uint8_t cmd) {
	uint16_t muxedVal = (uint16_t(id) << BITS_CMD_TRAME) | uint16_t(cmd);
	return (muxedVal & 0xFF);
}

uint8_t Trame::multiplex_cmd(uint8_t id, uint8_t cmd) {
	uint16_t muxedVal = (uint16_t(id) << BITS_CMD_TRAME) | uint16_t(cmd);
	return ((muxedVal >> 8) & 0xFF);
}
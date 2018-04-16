#include "trame.h"

Trame::Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number)
        : _id(id)
        , _cmd(cmd)
        , _data_length(data_length)
        , _packet_number(packet_number)
        , _data{0}
{
	if (data_length < 16) {
		memcpy(_data,data,data_length);
	}
	else {
		error("Invalid trame data length \n\r");
	}	

}

Trame::Trame() : _id(0), _cmd(0), _data_length(0), _packet_number(0), _data{0} {}

void Trame::send_ack(uint8_t packet_number,Serial* pc) {
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

	printf("Envoi du ack : ");
	for (uint8_t k; k < 8; k++) {
		printf("%#x ",tab[k]);
	}
	printf("\n\r");

	Buffer* buffer = new Buffer(tab,size);
	buffer->write(pc);
}


void Trame::sendToCan(Serial* pc) {
	int size = 8 + _data_length;
	uint8_t* tab = new uint8_t[size];

	// Remplir le tableau contenant la trame
	tab[0] = 0xAC;
	tab[1] = 0xDC;
	tab[2] = 0xAB;
	tab[3] = 0xBA;
	tab[4] = _packet_number;
	tab[5] = Trame::multiplexId(_id, _cmd);
	tab[6] = Trame::multiplexCmd(_id, _cmd);
	tab[7] = _data_length;
	for(int j = 0; j < _data_length; j++) {
		tab[8 + j] = _data[j];
	}
	Buffer* buffer = new Buffer(tab, size);
	buffer->write(pc);
}

uint8_t Trame::demultiplexId(uint8_t const& first, uint8_t const& second) {
	uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
	return uint8_t((muxedVal >> BITS_CMD_TRAME) & ~(1 << BITS_ID_TRAME));
}

uint8_t Trame::demultiplexCmd(uint8_t const& first, uint8_t const& second) {
	uint16_t muxedVal = (uint16_t(second) << 8) | uint16_t(first);
	return uint8_t(muxedVal & 0xF);
}

uint8_t Trame::multiplexId(uint8_t id, uint8_t cmd) {
	uint16_t muxedVal = (uint16_t(id) << BITS_CMD_TRAME) | uint16_t(cmd);
	return (muxedVal & 0xFF);
}

uint8_t Trame::multiplexCmd(uint8_t id, uint8_t cmd) {
	uint16_t muxedVal = (uint16_t(id) << BITS_CMD_TRAME) | uint16_t(cmd);
	return ((muxedVal >> 8) & 0xFF);
}

#include "trame.h"

Trame::Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number)
        : _id(id)
        , _cmd(cmd)
        , _data_length(data_length)
        , _packet_number(packet_number)
        , _data(data)
        , _write_callback(Callback<void(int)>(this, &Trame::deleteDataWrite))
        , _data_to_delete(NULL) {}

Trame::Trame() : _id(0), _cmd(0), _data_length(0), _packet_number(0), _data(nullptr) {}

void Trame::appendData(uint8_t data) {
	this->_data_length += 1;
	uint8_t* new_data = new uint8_t[this->_data_length];
	for(uint8_t i = 0; i < this->_data_length - 1; i++) {
		new_data[i] = this->_data[i];
	}
	delete this->_data;
	this->_data = new_data;
}

void Trame::deleteDataWrite(int event) {
	free(_data_to_delete);
}

void Trame::sendToCanAck(Serial* pc) const {
	// int size = HEADER_SIZE + _data_length;
	int size = 5;
	uint8_t* tab = new uint8_t[size];

	// Remplir le tableau contenant la trame
	tab[0] = 0xAC;
	tab[1] = 0xDC;
	tab[2] = 0xAB;
	tab[3] = 0xBB;
	tab[4] = _packet_number;

	// tab[size-1] = _packet_number;

	pc->write(tab, size, _write_callback, SERIAL_EVENT_TX_ALL);
}


void Trame::sendToCan(Serial* pc) const {
	// int size = HEADER_SIZE + _data_length;
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

	// tab[size-1] = _packet_number;

	pc->write(tab, size, _write_callback, SERIAL_EVENT_TX_ALL);
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

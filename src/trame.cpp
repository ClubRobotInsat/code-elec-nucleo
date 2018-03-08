#include "trame.h"

Trame::Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data)
        : _id(id), _cmd(cmd), _data_length(data_length), _data(data) {}

Trame::Trame() : _id(0), _cmd(0), _data_length(0), _data(nullptr) {}
void Trame::appendData(uint8_t data) {
	this->_data_length += 1;
	uint8_t* new_data = new uint8_t[this->_data_length];
	for(uint8_t i = 0; i < this->_data_length - 1; i++) {
		new_data[i] = this->_data[i];
	}
	delete this->_data;
	this->_data = new_data;
}

uint8_t* Trame::makeAck(uint8_t num_packet) {
	uint8_t* trame = new uint8_t[12];
	// TODO créer la trame de ACK
	return trame;
}
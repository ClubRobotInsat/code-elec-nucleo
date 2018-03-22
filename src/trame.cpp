#include "trame.h"

Trame::Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data, uint8_t packet_number)
        : _id(id)
        , _cmd(cmd)
        , _data_length(data_length)
        , _data(data)
        , _packet_number(packet_number)
        , _write_callback(Callback<void(int)>(this, &Trame::deleteDataWrite))
        , _data_to_delete(NULL) {}

Trame::Trame() : _id(0), _cmd(0), _data_length(0), _data(nullptr), _packet_number(0) {}

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

void Trame::deleteDataWrite(int event) {
	free(_data_to_delete);
}

void Trame::sendToCan(Serial *pc) const {
	int size = HEADER_SIZE + _data_length;
	uint8_t* tab = new uint8_t[this->_data_length];

	// Remplir le tableau contenant la trame
	tab[0] = _id;
	tab[1] = _cmd;
	tab[2] = _data_length;

	for(int i=0;i<_data_length;i++)
		tab[3+i] = _data[i];

	tab[size-1] = _packet_number;

	pc->write(tab,size,_write_callback,SERIAL_EVENT_TX_ALL);
}


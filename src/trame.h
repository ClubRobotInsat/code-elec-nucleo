#ifndef TRAME_H
#define TRAME_H

#include "mbed.h"

class Trame {

public:
	Trame(uint8_t id, uint8_t cmd, uint8_t data_length, uint8_t* data);
	Trame();

	uint8_t getData() const;
	uint8_t getId() const;
	uint8_t getDataLength() const;
	uint8_t getCmd() const;

	// ajoute une donnée à la trame
	void appendData(uint8_t data);

private :
	uint8_t _id, _cmd, _data_length;
	uint8_t* _data;
};

#endif

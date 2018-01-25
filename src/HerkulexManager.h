#ifndef HERKULEX_MANAGER_H
#define HERKULEX_MANAGER_H

#include <mbed.h>

#include "servo.h"
#include "bus.h"

template <uint8_t N_SERVOS> 
class HerkulexManager
{
private: 
	// The manager stores servos and the bus
	static HerkulexServo _servos[N_SERVOS];
	static uint8_t _nb_reg_servos; 

	static HerkulexBus _bus;

public: 
	HerkulexManager(); 
	virtual ~HerkulexManager(); 

	HerkulexServo & registerNewServo(uint8_t id);
	
}; 

#endif
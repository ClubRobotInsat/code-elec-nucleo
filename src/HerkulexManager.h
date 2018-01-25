#ifndef HERKULEX_MANAGER_H
#define HERKULEX_MANAGER_H

#include <mbed.h>

#include "HerkulexServo.h"
#include "HerkulexBus.h"
#include "GlobalMapping.h"

namespace herkulex
{
	template <uint8_t N_SERVOS> 
	class HerkulexManager
	{
	private: 
		// The manager stores servos
		static HerkulexServo _servos[N_SERVOS];

		// This member stores the number of registred servos. 
		// It must raise an error/a warning if _nb_reg_servos != N_SERVOS 
		static uint8_t _nb_reg_servos; 

		// The manager stores the bus - 
		static HerkulexBus _bus;

		// The state is refreshed at a defined period
		const uint32_t _refreshPeriod; 

		// The ticker calls a callback when the period is elapsed
		// Used to update all servos at a defined rate
		Ticker _it_ticker; 
	public: 
		HerkulexManager(uint32_t refreshTime); 
		virtual ~HerkulexManager(); 

		// This function is used to get a reference to a servo object with desired ID
		HerkulexServo & registerNewServo(uint8_t id);

	private: 
		static void updateServoState(uint8_t num_servo); 
		static void updateAllServos(); 
	}; 
}

// Templated source file, included in the header 
#include "HerkulexManager.tcc"

#endif
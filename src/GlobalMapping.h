#ifndef GLOBAL_MAPPING_H
#define GLOBAL_MAPPING_H

#include "mbed.h"

namespace global
{
	enum class PinMap 
	{
		HerkulexBusTXPin = 	11,  // Used to build static HerkulexBus in class HerkulexManager
		HerkulexBusRXPin = 	12,   // Used to build static HerkulexBus in class HerkulexManager
		PCSerialTXPin =    	USBTX, 
		PCSerialRXPin =		USBRX
	}
}

#endif
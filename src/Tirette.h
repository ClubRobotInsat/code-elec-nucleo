#include <mbed.h>


#ifndef TIRETTE_C
#define TIRETTE_C

class Tirette {

	public :
		Tirette(PinName pin);

		void send_io_trame(uint8_t data);

	private:
		InterruptIn _pin;
};



#endif

#ifndef BRUSHLESS_C
#define BRUSHLESS_C

#include "Trame.h"
#include <Servo.h>
#include <mbed.h>
enum BrushlessState { ON, OFF };


class Brushless {

public:
	explicit Brushless(PinName motor_pin);

	void set_brushless_state(BrushlessState state);

private:
	brushless::Servo _motor;
};

#endif

#include "Brushless.h"

Brushless::Brushless(PinName motor_pin) : _motor(motor_pin) {
	_motor = 0.0;
}

void Brushless::set_brushless_state(BrushlessState state) {
	switch(state) {
		case ON: {
			_motor = 1.0;
			break;
		}
		case OFF: {
			_motor = 0.0;
			break;
		}
	}
}

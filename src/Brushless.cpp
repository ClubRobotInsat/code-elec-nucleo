#include "Brushless.h"

Brushless::Brushless(PinName motor_pin) : _motor(motor_pin) {}

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

void Brushless::handle_trame(Trame t) {
	switch(t.get_cmd()) {
		case 0x03: {
			this->set_brushless_state(BrushlessState::ON);
		}
		case 0x04: {

			this->set_brushless_state(BrushlessState::OFF);
		}
	}
}

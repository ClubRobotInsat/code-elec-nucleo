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
		/*Moteur ON Brushless*/
		case 0x05: {
			this->set_brushless_state(BrushlessState::ON);
			break;
		}
		/*Moteur OFF*/
		case 0x04: {
			this->set_brushless_state(BrushlessState::OFF);
			break;
		}
		default:
			break;
	}
}

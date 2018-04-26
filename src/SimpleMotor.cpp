#include "SimpleMotor.h"

SimpleMotor::SimpleMotor(PinName dir, PinName pwm) : _pwm(pwm), _dir(dir) {}

void SimpleMotor::turn_on(Direction dir) {
	_dir.write(static_cast<uint8_t>(dir));
	_pwm.write(1.0f);
}

void SimpleMotor::turn_off() {
	_pwm.write(0.0f);
}

void SimpleMotor::handle_trame(Trame t) {
	switch(t.get_cmd()) {
		case 0x03: {
			uint8_t direction = t.get_data()[1];
			this->turn_on(static_cast<Direction>(direction));
			break;
		}
		case 0x04: {
			this->turn_off();
		}
	}
}

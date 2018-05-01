#include "Motor.h"


Motor::Motor(PinName pin_qei_1, PinName pin_qei_2, PinName pin_motor_control, PinName pin_direction_control)
        : _pulse_wanted(0)
        , _encoder(pin_qei_1, pin_qei_2, NC, PPR, QEI::X4_ENCODING)
        , _motor_speed_control(pin_motor_control)
        , _motor_direction_control(pin_direction_control)
        , _prev_error(0.0)
        , _error_sum(0.0) {
	_motor_speed_control.period(0.0001);
	_motor_speed_control.write(0);
	_motor_direction_control.write(0);
}

bool Motor::is_in_position() {
	int pulses = _encoder.getPulses();
	return _pulse_wanted >= pulses + MOTOR_DEAD_ZONE || _pulse_wanted <= pulses - MOTOR_DEAD_ZONE;
}

/* L'angle est en degré entre 0 et 360 !!!! */
void Motor::set_position(float angle, Direction dir) {
	// Reduction : 11.6 tours de codeur pour 1 tour de moteur
	float reduction = 11.6;
	float pulses = reduction * (float)PPR;
	if(dir == Direction::Clockwise) {
		_pulse_wanted = static_cast<int>((pulses / 360.0) * angle);
	} else {
		_pulse_wanted = -static_cast<int>((pulses / 360.0) * angle);
	}
}

void Motor::turn_n(int nb_turns, Direction dir) {
	float reduction = 11.6;
	float pulses = reduction * (float)PPR * (float)nb_turns;
	if(dir == Direction::Clockwise) {
		_pulse_wanted += static_cast<int>(pulses);
	} else {
		_pulse_wanted -= static_cast<int>(pulses);
	}
}

void Motor::asserv() {

	int pulses = _encoder.getPulses();

	if(_pulse_wanted >= pulses + MOTOR_DEAD_ZONE || _pulse_wanted <= pulses - MOTOR_DEAD_ZONE) {

		float error = abs(pulses - _pulse_wanted);
		float delta_error = fabs(error - _prev_error);
		_error_sum += error;
		_prev_error = error;


		// /!\ PWM >= MIN_PWM sinon le moteur ne tourne pas
		float pwm_value =
		    static_cast<float>(error * KP) + static_cast<float>(_error_sum * KI) + static_cast<float>(delta_error * KD);

		if(pwm_value < MOTOR_MIN_PWM) {
			pwm_value = MOTOR_MIN_PWM;
		}

		// debug("Je veut être en %d et je suis en %d => déplacement avec pour consigne
		// %f\n\r",_pulse_wanted,pulses,pwm_value);

		if(_pulse_wanted < pulses) {
			_motor_speed_control.write(pwm_value);
			_motor_direction_control.write(1);
		} else {
			_motor_speed_control.write(pwm_value);
			_motor_direction_control.write(0);
		}

	} else {
		// debug("Je suis en position ! \n\r");
		_motor_speed_control.write(0);
		_error_sum = 0;
		_prev_error = 0;
	}
}

float Motor::KP = 0.200 / 10000;
float Motor::KI = 0 / 10000;
float Motor::KD = 0 / 10000;

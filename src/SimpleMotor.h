#ifndef SIMPLE_MOTOR_H
#define SIMPLE_MOTOR_H

#include "Motor.h"
#include "Trame.h"
#include <mbed.h>

/* Un simple moteur non asservi qui se contente d'avoir deux états : ON ou OFF */
class SimpleMotor {

public:
	SimpleMotor(PinName pwm, PinName dir);

	void turn_off();
	void turn_on(Direction dir);
	void handle_trame(Trame t);

private:
	PwmOut _pwm;
	DigitalOut _dir;
};
#endif

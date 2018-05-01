#ifndef SIMPLE_MOTOR_H
#define SIMPLE_MOTOR_H

#include "Motor.h"
#include <mbed.h>

/* Un simple moteur non asservi qui se contente d'avoir deux états : ON ou OFF */
class SimpleMotor {

public:
	SimpleMotor(PinName dir, PinName pwm);

	void turn_off();
	void turn_on(Direction dir);

private:
	PwmOut _pwm;
	DigitalOut _dir;
};
#endif

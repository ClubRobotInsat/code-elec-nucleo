#ifndef MOTOR_ROBOT_H
#define MOTOR_ROBOT_H

#define PPR 4096

#include "Config.h"
#include "Trame.h"
#include <QEI.h>
#include <mbed.h>

enum class Direction {
	Clockwise = 0,
	Trigonometric = 1,
};

class Motor {
public:
	Motor(PinName pin_qei_1, PinName pin_qei_2, PinName pin_motor_control, PinName pin_direction_control);

	/* L'angle est en degré entre 0 et 360 !!!! */
	// TODO : Calculer le sens de rotation le plus rapide pour atteindre la position demandée !
	void set_position(float angle, Direction dir = Direction::Clockwise);

	void turn_n(int nb_turns, Direction dir);

	void asserv();

	bool is_in_position();

	static float KP;
	static float KI;
	static float KD;

private:
	int32_t _pulse_wanted;
	QEI _encoder;
	PwmOut _motor_speed_control;
	DigitalOut _motor_direction_control;
	float _prev_error;
	float _error_sum;
};

#endif

#define PPR 4096
#define MIN_PWM 0.08
#define MARGE 150

#include "QEI.h"
#include "mbed.h"

class Motor {
public:
	Motor(PinName pin_qei_1, PinName pin_qei_2, PinName pin_motor_control, PinName pin_direction_control, float period_asserv);

	/* L'angle est en degré entre 0 et 360 !!!! */
	void set_position(float angle);

	void turn_n(int nb_turns);

	void asserv();

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

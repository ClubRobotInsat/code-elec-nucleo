#include "Brushless.h"
#include "IDs_2018.h"
#include "Motor.h"
#include "SimpleMotor.h"
#include "Trame.h"
#include "Utils.h"
#include <mbed.h>

class Robot;

#ifndef MOTOR_2018_C
#define MOTOR_2018_C

class Motor2018 {

public:
	Motor2018(Motor* elevator_l, Motor* elevator_r, SimpleMotor* swallow_l, SimpleMotor* swallow_r, Brushless* _turbine_l, Brushless* _turbine_right);

	void asserv();

	void handle_trame(Trame t, Robot* robot);


private:
	void send_in_position_message(uint8_t motor_id, bool in_position);

	Motor* _elevator_l;
	Motor* _elevator_r;

	bool _in_position_l;
	bool _in_position_r;

	SimpleMotor* _swallow_l;
	SimpleMotor* _swallow_r;

	Brushless* _turbine_l;
	Brushless* _turbine_r;
};


#endif

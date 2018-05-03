#include "Motor2018.h"
#include "Robot.h"

Motor2018::Motor2018(Motor* elevator_l, Motor* elevator_r, SimpleMotor* swallow_l, SimpleMotor* swallow_r, Brushless* turbine_l, Brushless* turbine_r)
        :

        _elevator_l(elevator_l)
        , _elevator_r(elevator_r)
        , _in_position_l(true)
        , _in_position_r(true)
        , _swallow_l(swallow_l)
        , _swallow_r(swallow_r)
        , _turbine_l(turbine_l)
        , _turbine_r(turbine_r)


{}

void Motor2018::asserv() {
	_elevator_l->asserv();
	_elevator_r->asserv();

	if(_elevator_l->is_in_position() && !_in_position_l) {
		_in_position_l = true;
		send_in_position_message(ID_MOTOR_ELEVATOR_LEFT, true);
	}

	if(_elevator_r->is_in_position() && !_in_position_r) {
		send_in_position_message(ID_MOTOR_ELEVATOR_RIGHT, true);
		_in_position_r = true;
	}
}

void Motor2018::send_in_position_message(uint8_t motor_id, bool in_position) {
	uint8_t data[2];
	data[0] = motor_id;
	data[1] = static_cast<uint8_t>(in_position);
	Trame t(ID_ELEC_CARD_MOTORS, 0x06, 1, (uint8_t*)&data, 0);
	t.send_to_serial();
}

void Motor2018::handle_trame(Trame trame, Robot* robot) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds pong */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				robot->send_pong(ID_ELEC_CARD_MOTORS);
			}
			break;
		}
		/* Position en angle */
		case 0x01: {
			if(trame.get_data_length() == 5) {
				uint8_t motor_id = trame.get_data()[0];
				float angle = make_float(trame.get_data()[1], trame.get_data()[2], trame.get_data()[3], trame.get_data()[4]);
				switch(motor_id) {
					case ID_MOTOR_ELEVATOR_LEFT:
						_elevator_l->set_position(angle);
						_in_position_l = false;
						break;
					case ID_MOTOR_ELEVATOR_RIGHT:
						_elevator_r->set_position(angle);
						_in_position_r = false;
						break;
					default:
						break;
				}
			}
			break;
		}
		/* Position en tour */
		case 0x02: {
			if(trame.get_data_length() == 3) {
				uint8_t motor_id = trame.get_data()[0];
				uint8_t revolution = trame.get_data()[1];
				uint8_t direction = trame.get_data()[2];
				switch(motor_id) {
					case ID_MOTOR_ELEVATOR_LEFT:
						_elevator_l->turn_n(revolution, static_cast<Direction>(direction));
						_in_position_l = false;
						break;
					case ID_MOTOR_ELEVATOR_RIGHT:
						_elevator_r->turn_n(revolution, static_cast<Direction>(direction));
						_in_position_r = false;
						break;
					default:
						break;
				}
			}
			break;
		}
		/* Moteur ON */
		case 0x03: {
			if(trame.get_data_length() == 2) {
				uint8_t motor_id = trame.get_data()[0];
				uint8_t direction = trame.get_data()[1];
				switch(motor_id) {
					case ID_MOTOR_SWALLOW_LEFT:
						_swallow_l->turn_on(static_cast<Direction>(direction));
						break;
					case ID_MOTOR_SWALLOW_RIGHT:
						_swallow_r->turn_on(static_cast<Direction>(direction));
						break;
					case ID_MOTOR_BRUSHLESS_LEFT:
						_turbine_l->set_brushless_state(BrushlessState::ON);
						break;
					case ID_MOTOR_BRUSHLESS_RIGHT:
						_turbine_r->set_brushless_state(BrushlessState::ON);
						break;
					default:
						break;
				}
			}
			break;
		}
		/* Moteur OFF */
		case 0x04: {
			if(trame.get_data_length() == 1) {
				uint8_t motor_id = trame.get_data()[0];
				switch(motor_id) {
					case ID_MOTOR_SWALLOW_LEFT:
						_swallow_l->turn_off();
						break;
					case ID_MOTOR_SWALLOW_RIGHT:
						_swallow_r->turn_off();
						break;
					case ID_MOTOR_BRUSHLESS_LEFT:
						_turbine_l->set_brushless_state(BrushlessState::OFF);
						break;
					case ID_MOTOR_BRUSHLESS_RIGHT:
						_turbine_r->set_brushless_state(BrushlessState::OFF);
						break;
					default:
						break;
				}
			}
			break;
		}
		/* Moteur ON Brushless*/
		case 0x05: {
			if(trame.get_data_length() == 1) {
				uint8_t brushless_id = trame.get_data()[0];
				switch(brushless_id) {
					case ID_MOTOR_BRUSHLESS_LEFT:
						_turbine_l->set_brushless_state(BrushlessState::ON);
						break;
					case ID_MOTOR_BRUSHLESS_RIGHT:
						_turbine_r->set_brushless_state(BrushlessState::ON);
						break;
					default:
						break;
				}
			}
			break;
		}
		/* Lecture état des moteurs asservis */
		case 0x06: {
			if(trame.get_data_length() == 1) {
				uint8_t motor_id = trame.get_data()[0];
				switch(motor_id) {
					case ID_MOTOR_ELEVATOR_LEFT: {
						this->send_in_position_message(_elevator_l->is_in_position(), ID_MOTOR_ELEVATOR_LEFT);
						break;
					}
					case ID_MOTOR_ELEVATOR_RIGHT: {
						this->send_in_position_message(_elevator_r->is_in_position(), ID_MOTOR_ELEVATOR_RIGHT);
						break;
					}
				}
			}
			break;
		}
		default:
			break;
	}
}

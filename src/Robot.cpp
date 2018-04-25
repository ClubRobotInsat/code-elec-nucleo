#include "Robot.h"
#include "IDs_2018.h"
#include "Utils.h"

Robot::Robot()
        : _can(PA_11, PA_12, 500000)
        , _pc(USBTX, USBRX, 921600)
        , _servo_manager(A0, A1)
        , _turbine_left(PA_15)
        , _turbine_right(PA_10)
        , _motor_elevator_left(PB_3, PB_4, PA_8, PB_3, 0.05f)
        , _motor_elevator_right(PA_6, PA_7, PA_5, PB_4, 0.05f)
        , _motor_swallow_left(PB_8, PB_9) // FIXME
        , _motor_swallow_right(PB_9, PB_8)
        , _tirette(PA_9)
        , _trame_reader() {

	/* Activation des modules DMA */
	_pc.set_dma_usage_tx(DMA_USAGE_OPPORTUNISTIC);
	_pc.set_dma_usage_rx(DMA_USAGE_OPPORTUNISTIC);

	/* Activation de la reception des trames */
	_trame_reader.attach_to_serial(&_pc);

	/* Initialisation des servos (redémarrage) */
	for(uint8_t i = 0; i < 5; i++) {
		herkulex::Servo* servo = _servo_manager.register_new_servo(i);
		servo->reboot();
	}
	_servo_manager.flush_bus();

	// TODO : fixer le pullmode de la tirette
}

void Robot::send_pong(uint8_t id) {
	uint8_t data = 0xaa;
	Trame result = Trame(id, 0x00, 1, &data, 0);
	result.send_to_serial(&_pc);
}

void Robot::initialize_meca() {
	_turbine_left.set_brushless_state(BrushlessState::OFF);
	_turbine_right.set_brushless_state(BrushlessState::OFF);
}

void Robot::manage_robot() {

	if(_trame_reader.trame_ready()) {
		Trame trame = _trame_reader.get_trame();
		// Acquittement de la trame reçue.
		Trame::send_ack(trame.get_packet_number(), &_pc);
		this->handle_trame(trame);
	}

	/* Mise à jour des consignes d'asservissement des deux moteurs d'ascenseurs */
	_motor_elevator_left.asserv();
	_motor_elevator_right.asserv();
	/* Envoi des messages aux servomoteurs */
	_servo_manager.flush_bus();
}


void Robot::handle_trame(Trame trame) {
	switch(trame.get_id()) {
		case 0x00:
			handle_trame_nucleo(trame);
			break;
		case ID_ELEC_CARD_MOVEMENT:
			handle_trame_can(trame);
			break;
		case ID_ELEC_CARD_SERVOS:
			handle_trame_servo(trame);
			break;
		case ID_ELEC_CARD_IO:
			handle_trame_io(trame);
			break;
		case ID_ELEC_CARD_AVOIDANCE:
			handle_trame_can(trame);
			break;
		case ID_ELEC_CARD_MOTORS:
			handle_trame_motor(trame);
			break;
		default:
			break;
	}
}

void Robot::handle_trame_can(Trame trame) {
	_can.write(trame.into_can_message());
}

void Robot::handle_trame_motor(Trame trame) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds pong */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				this->send_pong(0x05);
			}
		}
		/* Position en angle */
		case 0x01: {
			if(trame.get_data_length() == 5) {
				uint8_t motor_id = trame.get_data()[0];
				switch(motor_id) {
					case ID_MOTOR_ELEVATOR_LEFT:
						_motor_elevator_left.handle_trame(trame);
						break;
					case ID_MOTOR_ELEVATOR_RIGHT:
						_motor_elevator_right.handle_trame(trame);
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
				switch(motor_id) {
					case ID_MOTOR_ELEVATOR_LEFT:
						_motor_elevator_left.handle_trame(trame);
						break;
					case ID_MOTOR_ELEVATOR_RIGHT:
						_motor_elevator_right.handle_trame(trame);
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
				switch(motor_id) {
					case ID_MOTOR_SWALLOW_LEFT:
						_motor_swallow_left.handle_trame(trame);
						break;
					case ID_MOTOR_SWALLOW_RIGHT:
						_motor_swallow_right.handle_trame(trame);
						break;
					case ID_MOTOR_BRUSHLESS_LEFT:
						_turbine_left.handle_trame(trame);
						break;
					case ID_MOTOR_BRUSHLESS_RIGHT:
						_turbine_right.handle_trame(trame);
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
						_motor_swallow_left.handle_trame(trame);
						break;
					case ID_MOTOR_SWALLOW_RIGHT:
						_motor_swallow_right.handle_trame(trame);
						break;
					case ID_MOTOR_BRUSHLESS_LEFT:
						_turbine_left.handle_trame(trame);
						break;
					case ID_MOTOR_BRUSHLESS_RIGHT:
						_turbine_right.handle_trame(trame);
						break;
					default:
						break;
				}
			}
			break;
		}
		default:
			break;
	}
}

void Robot::handle_trame_servo(Trame trame) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds Pong */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				this->send_pong(0x02);
			}
		}
		/* Réglage de la position */
		case 0x05: {
			if(trame.get_data_length() == 3) {
				uint8_t servo_id = trame.get_data()[0];
				uint16_t angle = (trame.get_data()[1] << 8) | trame.get_data()[2];
				if(angle <= 1023) {
					herkulex::Servo* serv = _servo_manager.get_servo_by_id(servo_id);
					if(serv != nullptr) {
						serv->set_position(angle);
					}
				}
			}
			break;
		}
		/* Demande de la position */
		case 0x06: {
			// TODO	(il faut d'abord fait la FSM pour la lecture des trames servos
		}
		default:
			break;
	}
}

void Robot::handle_trame_nucleo(Trame trame) {
	/* Gestion du ping pour la nucléo */
	if(trame.get_cmd() == 0x00 && trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
		this->send_pong(0x00);
	}
}

void Robot::handle_trame_io(Trame trame) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds PONG */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				this->send_pong(0x03);
			}
			break;
		}
		/* Lecture du pin de la tirette et réponse */
		case 0x01: {
			uint8_t data = _tirette.read();
			Trame result = Trame(ID_ELEC_CARD_IO, 0x01, 1, &data, 0);
			result.send_to_serial(&_pc);
			break;
		}
		default:
			break;
	}
};

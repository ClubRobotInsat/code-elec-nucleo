#include "Robot.h"
#include "Config.h"
#include "IDs_2018.h"
#include "Utils.h"

Robot::Robot()
        : _can(PA_11, PA_12, 500000)
        , _pc(PA_3, PA_2, BAUD_RATE_RPI_NUCLEO)
        , _servo_manager(PA_0, PA_1, BAUD_RATE_SERVO, DMA_USAGE_ROBOT)
        , _turbine_left(PB_15)
        , _turbine_right(PB_14)
        , _motor_elevator_left(PA_5, PA_6, PC_7, PA_9)
        , _motor_elevator_right(PC_14, PC_15, PC_13, PB_7)
        , _motor_swallow_left(PC_9, PC_8)
        , _motor_swallow_right(PB_9, PC_6)
        , _tirette(PC_2)
        , _trame_reader()
        , _trame_from_can_buffer() {

	/* Activation des modules DMA */
	_pc.set_dma_usage_tx(DMA_USAGE_ROBOT);
	_pc.set_dma_usage_rx(DMA_USAGE_ROBOT);

	/* Activation de la reception des trames depuis l'informatique */
	_trame_reader.attach_to_serial(&_pc);

	/* Activation de la reception des trames de puis le bus CAN */
	_can.attach(Callback<void()>(this, &Robot::read_trame_from_can), mbed::CAN::IrqType::RxIrq);


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

	/* Lecture des trames reçus depuis la connexion avec l'informatique */
	if(_trame_reader.trame_ready()) {
		Trame trame = _trame_reader.get_trame();
		// Acquittement de la trame reçue.
		Trame::send_ack(trame.get_packet_number(), &_pc);
		this->handle_trame(trame);
	}

	/* Lecture des trames reçus depuis le CAN */
	while(not _trame_from_can_buffer.empty()) {
		Trame trame;
		_trame_from_can_buffer.pop(trame);
		trame.send_to_serial(&_pc);
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

void Robot::read_trame_from_can() {
	CANMessage message_received;
	_can.read(message_received);
	Trame t(message_received);
	_trame_from_can_buffer.push(t);
	// TODO : delete le pointeur sur data de CANMessage ?
}

#include "Robot.h"


Robot::Robot()
        : _can(PA_11, PA_12, 500000)
        , _pc(USBTX, USBRX, 921600)
        , _servo_manager(A0, A1)
        , _turbine_left(PA_15)
        , _turbine_right(PA_10)
        , _motor_elevator_left(PB_3, PB_4, PA_8, PB_3, 0.05f)
        , _motor_elevator_right(PA_6, PA_7, PA_5, PB_4, 0.05f)
        , _motor_swallow_left(PB_8)
        , _motor_swallow_right(PB_9)
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
	_servo_manager.flush_bus();
}


void Robot::handle_trame(Trame trame) {
	switch(trame.get_id()) {
		case 0x00: {
			handle_trame_nucleo(trame);
		}
		case 0x02: {
			handle_trame_servo(trame);
			break;
		}
		case 0x03: {
			handle_trame_io(trame);
			break;
		}
		case 0x05: {
			handle_trame_motor(trame);
			break;
		}
		default: {
			handle_trame_can(trame);
			break;
		}
	}
}

void Robot::handle_trame_can(Trame trame) {}

void Robot::handle_trame_motor(Trame trame) {
	switch(trame.get_cmd()) {
		/* Position en angle */
		case 0x01: {
			if(trame.get_data_length() == 5) {
				uint8_t motor_id = trame.get_data()[0];
				// TODO
			}
			break;
		}
		/* Position en tour */
		case 0x02: {
			if(trame.get_data_length() == 3) {
				uint8_t motor_id = trame.get_data()[0];
				uint8_t revolution = trame.get_data()[1];
				uint8_t direction = trame.get_data()[2];
				// TODO
			}
			break;
		}
		/* Moteur ON */
		case 0x03: {
			if(trame.get_data_length() == 2) {
				uint8_t motor_id = trame.get_data()[0];
				uint8_t direction = trame.get_data()[1];
				// TODO
			}
			break;
		}
		/* Moteur OFF */
		case 0x04: {
			if(trame.get_data_length() == 2) {
				uint8_t motor_id = trame.get_data()[0];
				uint8_t direction = trame.get_data()[1];
				// TODO
			}
			break;
		}
		default:
			break;
	}
}

void Robot::handle_trame_servo(Trame trame) {}

void Robot::handle_trame_nucleo(Trame trame) {}

void Robot::handle_trame_io(Trame trame){};

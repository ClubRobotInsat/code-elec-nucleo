#include "Robot.h"


Robot::Robot()
        : _can(PA_11, PA_12, 500000)
        , _pc(USBTX, USBRX, 921600)
        , _servo_manager(A0, A1, 2)
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

	/* Activation de la récéption des trames */
	_trame_reader.attach_to_serial(&_pc);
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
	_servo_manager.flushBus();
}


void Robot::handle_trame(Trame trame) {
	switch(trame.getId()) {
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
void Robot::handle_trame_motor(Trame trame) {}
void Robot::handle_trame_servo(Trame trame) {}
void Robot::handle_trame_nucleo(Trame trame) {}
void Robot::handle_trame_io(Trame trame){};

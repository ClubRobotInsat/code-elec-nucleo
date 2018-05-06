#include "Robot.h"
#include "Config.h"
#include "IDs_2018.h"
#include "IDs_2018.h"
#include "Utils.h"

Robot::Robot()
        : _can(PA_11, PA_12, 500000)
        , _pc(PC_10, PC_11, BAUD_RATE_RPI_NUCLEO)
        , _servo_manager(PA_0, PA_1, BAUD_RATE_SERVO, DMA_USAGE_ROBOT)
        , _turbine_left(PB_15)
        , _turbine_right(PB_14)
        , _motor_elevator_left(PA_5, PA_6, PA_9, PC_7)
        , _motor_elevator_right(PC_14, PC_15, PB_7, PC_13)
        , _motor_swallow_left(PC_9, PC_8)
        , _motor_swallow_right(PB_9, PC_6)
        , _tirette(PC_2)
        , _trame_reader()
        , _trame_from_can_buffer()
        , _motor_logic(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
        , _tirette_detected(false) {

	debug("Activation du robot ...\n\r");
	_motor_logic =
	    Motor2018(&_motor_elevator_left, &_motor_elevator_right, &_motor_swallow_left, &_motor_swallow_right, &_turbine_left, &_turbine_right);

	/* Activation des modules DMA */
	_pc.set_dma_usage_tx(DMA_USAGE_ROBOT);
	_pc.set_dma_usage_rx(DMA_USAGE_ROBOT);

	/* Activation de la reception des trames depuis l'informatique */
	_trame_reader.attach_to_serial(&_pc);

	/* Activation de la reception des trames de puis le bus CAN */
	_can.attach(Callback<void()>(this, &Robot::read_trame_from_can), CAN::IrqType::RxIrq);


	/* Initialisation des servos (redémarrage) */
	for(uint8_t i = 1; i <= 4; i++) {
		herkulex::Servo* servo = _servo_manager.register_new_servo(i);
		servo->reboot();
	}
	/// debug
	// herkulex::Servo* servo = _servo_manager.register_new_servo(0xFD);
	// servo->reboot();

	_servo_manager.flush_bus();

	_tirette.mode(PinMode::PullDown);
	_tirette.fall(Callback<void()>(this, &Robot::callback_tirette));
}

void Robot::send_pong(uint8_t id) {
	uint8_t data = 0xaa;
	Trame result = Trame(id, 0x00, 1, &data, 0);
	result.send_to_serial();
}

void Robot::initialize_meca() {
	_turbine_left.set_brushless_state(BrushlessState::OFF);
	_turbine_right.set_brushless_state(BrushlessState::OFF);
	_servo_manager.get_servo_by_id(0xFD)->enable_torque(true);
	_servo_manager.flush_bus();
	wait_ms(10);
	_servo_manager.get_servo_by_id(0xFD)->set_position(256);
}

void Robot::callback_tirette() {
	_tirette_detected = true;
}

void Robot::manage_robot() {
	_trame_reader.parse_buffer();
	/* Lecture des trames reçues depuis la connexion avec l'informatique */
	while(_trame_reader.trame_ready()) {
		// debug("Trame reçue \n\r!");
		Trame trame = _trame_reader.get_trame();
		// Acquittement de la trame reçue.
		Trame::send_ack(trame.get_packet_number(), &_pc);
		this->handle_trame(trame);
	}

	/* Lecture des trames reçus depuis le CAN */
	while(not _trame_from_can_buffer.empty()) {
		Trame trame;
		_trame_from_can_buffer.pop(trame);
		trame.send_to_serial();
	}

	if(_tirette_detected) {
		_tirette_detected = false;
		uint8_t data[2];
		data[0] = ID_CONTACTOR_TIRETTE;
		data[1] = 1;
		Trame result = Trame(ID_ELEC_CARD_IO, 0x01, 2, (uint8_t*)&data, 0);
		result.send_to_serial();
	}

	/* Mise à jour des consignes d'asservissement des deux moteurs d'ascenseurs */
	_motor_logic.asserv();

	//_servo_manager.get_servo_by_id(0xFD)->set_position(256);
	/* Envoi des messages aux servomoteurs */
	_servo_manager.flush_bus();
	Buffer::flush_buffers(&_pc);
}


void Robot::handle_trame(Trame trame) {
	// printf("Handling : ");
	// print_trame(trame);
	// printf("\n\r");
	switch(trame.get_id()) {
		case ID_NUCLEO:
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
			_motor_logic.handle_trame(trame, this);
			break;
		default:
			break;
	}
}

void Robot::handle_trame_can(Trame trame) {
	_can.write(trame.into_can_message());
}

void Robot::handle_trame_servo(Trame trame) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds Pong */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				this->send_pong(ID_ELEC_CARD_SERVOS);
			}
			break;
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
		this->send_pong(ID_NUCLEO);
	}
}

void Robot::handle_trame_io(Trame trame) {
	switch(trame.get_cmd()) {
		/* Ping : on réponds PONG */
		case 0x00: {
			if(trame.get_data_length() == 1 && trame.get_data()[0] == 0x55) {
				this->send_pong(ID_ELEC_CARD_IO);
			}
			break;
		}
		/* Lecture du pin de la tirette et réponse */
		case 0x01: {
			uint8_t data[2];
			data[0] = ID_CONTACTOR_TIRETTE; // Numéro de capteur de la tirette.
			data[1] = _tirette.read();
			Trame result = Trame(ID_ELEC_CARD_IO, 0x01, 2, (uint8_t*)&data, 0);
			result.send_to_serial();
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

void Robot::print_trame(const Trame& trame) {
	// printf("id : %#x | cmd %#x | (%d) ", trame.get_id(), trame.get_cmd(), trame.get_data_length());
	for(int i = 0; i < trame.get_data_length(); i++) {
		// printf("%#x ", trame.get_data()[i]);
	}
}

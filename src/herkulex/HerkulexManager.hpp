namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin)
	        : _bus(txPin, rxPin, BUS_BAUDRATE, 0.05), _nb_reg_servos(0) {}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() {
		for(uint8_t i = 0; i < _nb_reg_servos; i++) {
			delete _servos[i];
		}
	}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::register_new_servo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) {

			// On incremente le nombre de servos
			_nb_reg_servos++;
			// _nb_reg_servos >= 1 ici

			// On cree un nouveau servo, que l'on stocke
			// NEW -> DELETE dans ~Manager
			_servos[_nb_reg_servos - 1] = new Servo(id, &_bus);

			// debug("Registering : ID %#x (%d/%d) \n\r", id, _nb_reg_servos, N_SERVOS);


			// Et on le retourne
			return _servos[_nb_reg_servos - 1];
		} else {
			// debug("Trop de servos !!!\n\r");
			/// FIXME : LE NULLPTR
			return nullptr;
		}
	}

	//	template <uint8_t N_SERVOS>
	//	void Manager<N_SERVOS>::sendUpdatesToNextServo() {
	//		/*if(_bus.needFlush()) {
	//		    //debug("Bus need to be flushed\n\r");
	//		    //_bus.flush();
	//		    return;
	//		}*/
	//		// Iterate on each servo
	//		/* First thing to do in the callback, because incrementation of _num_next_servo
	//		 * has big side effects : all the answers from the previous servo (let say nb. i)
	//		 * will now be ignored. Its time is over, it loses his token and is no listened
	//		 * until the next "loop".
	//		 * The next servo (i + 1) has now the token.
	//		 */
	//		// debug("Updating servo n°%d", _num_next_servo);
	//		if(_num_next_servo < _nb_reg_servos - 1) {
	//			++_num_next_servo;
	//		} else {
	//			_num_next_servo = 0;
	//		}
	//
	//		Servo* s = _servos[_num_next_servo];
	//
	//		// debug(" with id %x...\n\r", s->getId());
	//
	//		if(s->shouldReboot()) {
	//			// debug("Rebooting (%x).\n\r", s->getId());
	//			_bus.sendRebootMsg(s->getId());
	//			//_bus.flush();
	//			wait_ms(20);
	//			s->_should_reboot = false;
	//			return;
	//		}
	//
	//		// Enable ACK policy
	//		//_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::AckPolicy, 0x02);
	//		// !!! TODO !!! See if it is better to use Calibrated or AbsolutePosition
	//		//_bus.readRAMAddr(s->_id, constants::RAMAddr::CalibratedPosition, 2, &_callback_update_servo);
	//		// Enable/Disable torque to match with s->_desired_torque_on
	//		if(s->_desired_torque_on && !(s->isTorqueOn())) {
	//			_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::TorqueControl, constants::TorqueControl::TorqueOn);
	//		} else if(!(s->_desired_torque_on) && s->isTorqueOn()) {
	//			_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::TorqueControl, constants::TorqueControl::TorqueFree);
	//		}
	//
	//		constants::LedColor::LedColorEnum led_color;
	//		// Select led color
	//
	//		if(s->_status_detail & constants::StatusDetail::InpositionFlag)
	//			led_color = s->_inposition_led_color;
	//		else
	//			led_color = s->_moving_led_color;
	//		// ?? PLAYTIME ??
	//		_bus.sendSJOGMsg(s->_id, constants::jog_default_playtime, s->_desired_position,
	//constants::JOG_CMD::PositionMode | 0x04);
	//		//_bus.sendRAMWriteMsg(s->getId(), constants::RAMAddr::LedControl,static_cast<uint8_t>(led_color),1,0);
	//		// Check and clear the status if needed
	//		/*
	//		if(s->_status_error != 0x00) {
	//		    //debug("Trying to clear status (%x) of servo #%x\n\r", s->_status_error, s->_id);
	//		    _bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::StatusError, 0x00);
	//		}
	//		*/
	//	}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::get_servo_by_id(uint8_t id) {
		for(uint8_t i = 0; i < _nb_reg_servos; i++) {
			if(_servos[i]->_id == id) {
				return _servos[i];
			}
		}
		return nullptr;
	}

	template <uint8_t N_SERVOS>
	void Manager<N_SERVOS>::send_debug_message() {
		_bus.send_debug_message();
	}

	template <uint8_t N_SERVOS>
	void Manager<N_SERVOS>::flush_bus() {
		_bus.flush();
	}
}

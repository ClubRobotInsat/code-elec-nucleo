namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, float refreshPeriod, Serial* pc) : 
		_bus(txPin, rxPin, pc), 
		_log(pc), 
		_it_ticker(), 
		_nb_reg_servos(0),
		_num_next_servo(-1), 
		_refreshPeriod(refreshPeriod), 
		_callback_update_servo_status(this, &Manager<N_SERVOS>::cbFuncUpdateServoStatus),
		_callback_update_servo(this, &Manager<N_SERVOS>::cbFuncUpdateServo)
	{}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() {
		for(uint8_t i = 0; i < _nb_reg_servos; i++)
		{
			delete _servos[i];
		}
	}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::registerNewServo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) 
		{

			// On incremente le nombre de servos 
			_nb_reg_servos++; 
			// _nb_reg_servos >= 1 ici

			// On cree un nouveau servo, que l'on stocke 
			// NEW -> DELETE dans ~Manager
			_servos[_nb_reg_servos - 1] = new Servo(id, _log);

			// On lance le ticker pour manager les servos !
			if(_nb_reg_servos == 1)
			{
				_log->printf("Starting ticker for the manager, with speed : %f \n\r",_refreshPeriod/N_SERVOS);
				_it_ticker.attach(Callback<void()>(this, &Manager<N_SERVOS>::cbSendUpdatesToNextServo), 
						_refreshPeriod / N_SERVOS);
			}

			// Et on le retourne 
			return _servos[_nb_reg_servos - 1];
		} 
		else 
		{
			_log->printf("Trop de servos !!!\n\r");
			/// FIXME : LE NULLPTR
			return nullptr;
		}
	}

	template <uint8_t N_SERVOS>
	void Manager<N_SERVOS>::cbSendUpdatesToNextServo() 
	{
		// Iterate on each servo
		/* First thing to do in the callback, because incrementation of _num_next_servo 
		 * has big side effects : all the answers from the previous servo (let say nb. i) 
		 * will now be ignored. Its time is over, it loses his token and is no listened 
		 * until the next "loop". 
		 * The next servo (i + 1) has now the token. 
		 */ 
		_log->printf("Updating servo n°%d",_num_next_servo);
		if(_num_next_servo < _nb_reg_servos - 1) 
			++_num_next_servo; 
		else
			_num_next_servo = 0;

		Servo * s = _servos[_num_next_servo]; 

		_log->printf(" with id %x...\n\r",s->getId());

		if (s->shouldReboot()) {
			_log->printf("Rebooting (%x).\n\r",s->getId());
			_bus.sendRebootMsg(s->getId());
			s->_should_reboot=false;
			return;
		}

		// !!! TODO !!! See if it is better to use Calibrated or AbsolutePosition
		/*_bus.readRAMAddr(s->_id, constants::RAMAddr::AbsolutePosition, 2, &_callback_update_servo); 

		// Enable/Disable torque to match with s->_desired_torque_on
		if( s->_desired_torque_on && !(s->isTorqueOn()) )
		{
			_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::TorqueControl, 
				constants::TorqueControl::TorqueOn); 
		}
		else if( !(s->_desired_torque_on) && s->isTorqueOn() )
		{
			_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::TorqueControl, 
				constants::TorqueControl::TorqueFree); 
		}

		*/
		constants::LedColor::LedColorEnum led_color; 

		// Select led color
		if(s->_status_detail & constants::StatusDetail::InpositionFlag) 
			led_color = s->_inposition_led_color; 
		else
			led_color = s->_moving_led_color; 

		// ?? PLAYTIME ?? 
		_bus.sendSJOGMsg(s->_id, constants::jog_default_playtime, s->_desired_position, 
				constants::JOG_CMD::PositionMode | led_color); 
		_bus.sendRAMWriteMsg(s->getId(), constants::RAMAddr::LedControl,static_cast<uint8_t>(led_color),1,0);

		// Check and clear the status if needed
		if(s->_status_error != 0x00)
		{
			_log->printf("Trying to clear status (%x) of servo #%x\n", s->_status_error, s->_id);
			_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::StatusError, 0x00);
		}
	}

	template <uint8_t N_SERVOS>
	void Manager<N_SERVOS>::cbFuncUpdateServoStatus(uint8_t id, uint8_t status_error, uint8_t status_detail)
	{

		if(_servos[_num_next_servo]->_id != id)
		{
			_log->printf("Recu un update pour le mauvais servo...\n");
		}
		else
		{
			_servos[_num_next_servo]->mgrUpdateStatus(status_error, status_detail);
		}
	}

	template <uint8_t N_SERVOS>
	void Manager<N_SERVOS>::cbFuncUpdateServo(uint8_t id, uint8_t status_error, uint8_t status_detail,
			uint8_t data0, uint8_t data1)
	{
		if(_servos[_num_next_servo]->_id != id)
		{
			_log->printf("Recu un update pour le mauvais servo...\n");
		}
		else
		{
			// Position : 2 first bits from msb (data1) + data0 (0~1023)
			_servos[_num_next_servo]->mgrUpdatePosition(( (data1 & 0x03) << 8 ) | data0 );
			_servos[_num_next_servo]->mgrUpdateStatus(status_error, status_detail); 
		}
	}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::getServoById(uint8_t id) {
		for (uint8_t i = 0; i < _nb_reg_servos; i++) {
			if (_servos[i]->_id == id) {
				return _servos[i];
			}
		}
		return nullptr;
	}
}

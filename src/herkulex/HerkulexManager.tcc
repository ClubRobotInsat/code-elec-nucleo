namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, us_timestamp_t refreshPeriod, Serial* pc) : 
		_bus(txPin, rxPin, pc), 
		_log(pc), 
		_it_ticker(), 
		_nb_reg_servos(0),
		_num_next_servo(0), 
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
			_log->printf("Tiens voilà un servo\n");

			// On incremente le nombre de servos 
			_nb_reg_servos++; 
			// _nb_reg_servos >= 1 ici

			// On cree un nouveau servo, que l'on stocke 
			// NEW -> DELETE dans ~Manager
			_servos[_nb_reg_servos - 1] = new Servo(id, _bus, _log);

			// On lance le ticker pour manager les servos !
			if(_nb_reg_servos == 1)
			{
				_it_ticker.attach(Callback<void()>(this, &Manager<N_SERVOS>::cbSendUpdatesToNextServo), 
						_refreshPeriod / N_SERVOS);
			}

			// Et on le retourne 
			return _servos[_nb_reg_servos - 1];
		} 
		else 
		{
			_log->printf("Trop de servos !!!\n");
			/// FIXME : LE NULLPTR
			return nullptr;
		}
	}

	template <uint8_t N_SERVOS>
		void Manager<N_SERVOS>::cbSendUpdatesToNextServo() {
			Servo * s = _servos[_num_next_servo]; 

			// !!! TODO !!! See if it is better to use Calibrated or AbsolutePosition
			_bus.readRAMAddr(s->_id, constants::RAMAddr::AbsolutePosition, 2, &_callback_update_servo); 

			// ?? PLAYTIME ?? 
			_bus.sendSJOGMsg(s->_id, constants::jog_default_playtime, s->_desired_position, 
					constants::JOG_CMD::PositionMode | constants::JOG_CMD::GreenLedOn); 

			// Check and clear the status if needed
			if(s->_status_error != 0x00)
			{
				_log->printf("Trying to clear status (%x) of servo #%x\n", s->_status_error, s->_id);
				_bus.sendRAMWriteMsg(s->_id, constants::RAMAddr::StatusError, 0x00);
			}

			// Iterate on each servo
			if(_num_next_servo < _nb_reg_servos - 1) 
				++_num_next_servo; 
			else
				_num_next_servo = 0;
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

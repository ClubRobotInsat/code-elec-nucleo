namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, us_timestamp_t refreshPeriod, Serial* pc) : 
		_bus(txPin, rxPin, pc), 
	    _log(pc), 
	    _it_ticker(), 
	    _nb_reg_servos(0),
	    _num_next_servo(0), 
	    _refreshPeriod(refreshPeriod)
	{}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() 
	{
		for(uint8_t i = 0; i < _nb_reg_servos; ++i)
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
				_it_ticker.attach(this, sendUpdatesToNextServo, refreshPeriod / N_SERVOS);
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
	void Manager<N_SERVOS>::sendUpdatesToNextServo() {
		Servo * s = _servos[_num_next_servo]; 

		// Read status
		_bus.sendStatMsg(s->_id); // A remplacer par des fetch non bloquants auxquels on passe un callback 

		// ?? PLAYTIME ?? 
		_bus.sendSJOGMsg(s->_id, constants::jog_default_playtime, s->_desired_position, 
			constants::JOG_CMD::PositionMode | constants::JOG_CMD::GreenLedOn); 

		// Read calibrated position 
		_bus.sendRAMReadMsg(s->id, constants::RAMAddr::CalibratedPosition, 2); 
	}
}

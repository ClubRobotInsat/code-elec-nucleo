namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, uint32_t refreshTime, Serial* pc)
	        : _bus(txPin, rxPin, pc), _refreshPeriod(refreshTime), _log(pc), _nb_reg_servos(0) {}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() {}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::registerNewServo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) 
		{
			_log->printf("Tiens voilà un servo\n");

			// On incremente le nombre de servos 
			_nb_reg_servos++; 
			// _nb_reg_servos >= 1 ici
			
			// On cree un nouveau servo, que l'on stocke 
			_servos[_nb_reg_servos - 1] = new Servo(id, _bus, _log);

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
}

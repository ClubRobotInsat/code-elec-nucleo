namespace herkulex {

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, uint32_t refreshTime, Serial* pc)
	        : _bus(txPin, rxPin, pc), _refreshPeriod(refreshTime), _log(pc), _nb_reg_servos(0) {}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() {
        _log->printf("Destruction du manager \n");
    }

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::registerNewServo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) {
			_servos[_nb_reg_servos] = _bus.makeNewServo(id);
			_nb_reg_servos++;
			_log->printf("Tiens voilà un servo\n");
			return _servos[_nb_reg_servos-1];
		} else {
			_log->printf("Trop de servos !!!\n");
			/// FIXME : LE NULL
			return NULL;
		}
	}
}

namespace herkulex {
	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::_servos[N_SERVOS];

	template <uint8_t N_SERVOS>
	uint8_t Manager<N_SERVOS>::_nb_reg_servos = 0;

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::Manager(PinName txPin, PinName rxPin, uint32_t refreshTime, Serial* pc)
	        : _bus(txPin, rxPin, pc), _refreshPeriod(refreshTime), _log(pc) {}

	template <uint8_t N_SERVOS>
	Manager<N_SERVOS>::~Manager() {}

	template <uint8_t N_SERVOS>
	Servo* Manager<N_SERVOS>::registerNewServo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) {
			_servos[_nb_reg_servos] = new Servo(id, _bus, _log);
			_nb_reg_servos++;
			return _servos[_nb_reg_servos];
		} else {
			/// FIXME : LE NULLPTR
			return nullptr;
		}
	}
}

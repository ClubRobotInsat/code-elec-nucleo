#include "HerkulexManager.h"

namespace herkulex {
	template <uint8_t N_SERVOS>
	HerkulexServo HerkulexManager<N_SERVOS>::_servos[N_SERVOS];

	template <uint8_t N_SERVOS>
	uint8_t HerkulexManager<N_SERVOS>::_nb_reg_servos = 0;

	template <uint8_t N_SERVOS>
	HerkulexManager<N_SERVOS>::HerkulexManager(uint8_t txPin, uint8_t rxPin, uint32_t refreshTime)
	        : _bus(txPin, rxPin), _refreshPeriod(refreshTime) {}

	template <uint8_t N_SERVOS>
	HerkulexManager<N_SERVOS>::~HerkulexManager() {}

	template <uint8_t N_SERVOS>
	HerkulexServo& HerkulexManager<N_SERVOS>::registerNewServo(uint8_t id) {
		if(_nb_reg_servos < N_SERVOS) {
			_servos[_nb_reg_servos] = HerkulexServo(id, _bus);
			_nb_reg_servos++;
			return &_servos[_nb_reg_servos];
		} else {
			return nullptr;
		}
	}
}

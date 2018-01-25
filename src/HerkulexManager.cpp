#include "HerkulexManager.h"

template <uint8_t N_SERVOS>
HerkulexServo HerkulexManager<N_SERVOS>::_servos[N_SERVOS];

template <uint8_t N_SERVOS>
HerkulexBus HerkulexManager<N_SERVOS>::_bus;

template <uint8_t N_SERVOS>
uint8_t HerkulexManager<N_SERVOS>::_nb_reg_servos = 0; 

template <uint8_t N_SERVOS>
HerkulexManager<N_SERVOS>::HerkulexManager() 
{

}

template <uint8_t N_SERVOS>
HerkulexManager<N_SERVOS>::~HerkulexManager() 
{

}

template <uint8_t N_SERVOS> 
HerkulexServo & HerkulexManager<N_SERVOS>::registerNewServo(uint8_t id)
{

}
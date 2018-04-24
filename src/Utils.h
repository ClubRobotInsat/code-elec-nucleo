#ifndef UTILS_ROBOT_H
#define UTILS_ROBOT_H

#include <mbed.h>

float make_float(uint8_t first, uint8_t second, uint8_t third, uint8_t last) {
	uint32_t temp = ((first << 24) | (second << 16) | (third << 8) | last);
	return static_cast<float>(temp);
}

uint16_t make_u16(uint8_t msb, uint8_t lsb) {
	return (msb >> 8) | lsb;
}


#endif

#ifndef UTILS_ROBOT_H
#define UTILS_ROBOT_H

#include <mbed.h>

float make_float(uint8_t first, uint8_t second, uint8_t third, uint8_t last);

uint16_t make_u16(uint8_t msb, uint8_t lsb);

uint8_t lsb(uint16_t data);

uint8_t msb(uint16_t data);


#endif

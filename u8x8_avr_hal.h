#ifndef _U8X8_AVR_HAL_H_
#define _U8X8_AVR_HAL_H_

#include "u8x8.h"


#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>

//Function prototypes



unsigned char u8x8_byte_avr_hw_i2c(u8x8_t *u8x8, unsigned char msg, unsigned char arg_int, void *arg_ptr);

unsigned char u8x8_gpio_and_delay_avr(u8x8_t *u8x8, unsigned char msg, unsigned char arg_int, void *arg_ptr);

#endif

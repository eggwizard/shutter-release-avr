#ifndef _PULSE_GENERATOR_H_
#define _PULSE_GENERATOR_H_

#include <avr/io.h>

#define PULSE_GEN_CMD_RUN 1
#define PULSE_GEN_CMD_RESET 2


#define PULSE_PORT      PORTD
#define PULSE_PORT_DDR  DDRD
#define PULSE_PIN_MAIN  3
#define PULSE_PIN_SUB   4


int pulse_generator_100ms_unit(int high_duration, int period, int count, unsigned char cmd);

#endif
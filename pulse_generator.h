#ifndef _PULSE_GENERATOR_H_
#define _PULSE_GENERATOR_H_

#include <avr/io.h>

#define PULSE_GEN_CMD_RUN 1
#define PULSE_GEN_CMD_RESET 2

#define WAKEUP_PREAMBLE_MS 2500
#define POWER_SLEEP_SEC 10

#define PULSE_PORT      PORTD
#define PULSE_PORT_DDR  DDRD
#define PULSE_PIN_MAIN  3//3
#define PULSE_PIN_SUB   4

#define PULSE_STATE_STANBY	0
#define PULSE_STATE_HIGH	1
#define PULSE_STATE_LOW	2
#define PULSE_STATE_PREAMBLE	3


int pulse_gen_w_preamble_100ms_unit(int count, int high_duration, int period, int preamble, unsigned char cmd);

int pulse_gen_100ms_unit(int count, int high_duration, int period, unsigned char cmd);


#endif

#ifndef _CLOCK_H_
#define _CLOCK_H_


// #define PULSE_GEN_CMD_RUN 1
// #define PULSE_GEN_CMD_RESET 2


// #define PULSE_PORT      PORTD
// #define PULSE_PORT_DDR  DDRD
// #define PULSE_PIN_MAIN  3
// #define PULSE_PIN_SUB   4

#define CLOCK_TICK_COUNTER_MAX 10000
#define CLOCK_SEC_COUNTER_MAX 20000
#define CLOCK_100MS_COUNTER_MAX 30000

#define CLOCK_HW_COUNTER_TOP_VAL 250
#define CLOCK_SUB_PRESCALER 5

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#if defined(__AVR_ATtiny85__)
#define CLOCK_TIMER_COMP_VECT TIMER0_COMPA_vect
#endif

#if defined(__AVR_ATmega8__)
#define CLOCK_TIMER_COMP_VECT TIMER2_COMP_vect
#endif

#if defined(__AVR_ATmega88__)
#define CLOCK_TIMER_COMP_VECT TIMER0_COMPA_vect
#endif

void install_timer0(void (*fctparam)(void), unsigned int);

unsigned char is_timer_btn_alive(void);
void kill_timer_btn(void);
void install_timer_btn(void (*fctparam)(void), unsigned int);
unsigned int get_tick_counter_delta(void);
void init_clock(void);
void tic(void);
unsigned int toc(void);
unsigned int get_tick_counter(void);
unsigned int get_tick_time(void);
unsigned int get_100ms_counter(void);

#endif


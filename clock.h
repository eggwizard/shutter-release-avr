#ifndef _CLOCK_H_
#define _CLOCK_H_


#define CLOCK_COUNTER_MAX 10000

#define CLOCK_COUNTER_TOP_VAL 250
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

void install_timer0(void (*fctparam)(void), unsigned int);

unsigned char is_timer_btn_alive(void);
void kill_timer_btn(void);
void install_timer_btn(void (*fctparam)(void), unsigned int);

void init_clock(void);
void tic(void);
unsigned int toc(void);
unsigned int get_tick_counter(void);
unsigned int get_tick_time(void);

#endif


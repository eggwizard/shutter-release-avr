
#include "clock.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define CLOCK_MAIN_PRESCALER 64

static unsigned int counter = 0;
static unsigned int counter_tic = 0;

static unsigned int counter_sub = 0;

static void (*fct_timer0_callback)(void);
static int timer0_set_time_tick = 0;

static void (*fct_timer_btn_callback)(void);
static int timer_btn_set_time_tick = 0;


void install_timer0(void (*fctparam)(void), unsigned int set_time_ms){

	fct_timer0_callback = fctparam;
	timer0_set_time_tick = set_time_ms / get_tick_time();
	
}

unsigned char is_timer_btn_alive(void){
	if (timer_btn_set_time_tick>0) {
		return 1;
	} else {
		return 0;
	}
}

void kill_timer_btn(void){
	timer_btn_set_time_tick = 0;
}

void install_timer_btn(void (*fctparam)(void), unsigned int set_time_ms){

	fct_timer_btn_callback = fctparam;
	timer_btn_set_time_tick = set_time_ms / get_tick_time();
	
}

void init_clock(void){

#if defined(__AVR_ATtiny85__)
	//Use Timer/counter2
	OCR0A = CLOCK_COUNTER_TOP_VAL; // output compare matchc occured in every 2ms
	TCCR0A |= (1<<WGM01); // prescale factor = 64, 8us per minor tick
	TCCR0B |= (1<<CS01)|(1<<CS00);	
	TIMSK |= (1<<OCIE0A);

#endif

#if defined(__AVR_ATmega8__)
	OCR2 = CLOCK_COUNTER_TOP_VAL;
	TCCR2 |= (1<<WGM21)|(1<<CS22);
	TIMSK |= (1<<OCIE2);
		
#endif

#if defined(__AVR_ATmega88__)
	OCR0A = CLOCK_COUNTER_TOP_VAL; // output compare matchc occured in every 2ms
	TCCR0A |= (1<<WGM01); // prescale factor = 64, 8us per minor tick
	TCCR0B |= (1<<CS01)|(1<<CS00);	
	TIMSK0 |= (1<<OCIE0A);
#endif
}

void tic(void){
	counter_tic = counter;
}

unsigned int toc(void){
	
	unsigned int dtick = 0;
	unsigned long counter_temp = 0;

	counter_temp = counter;
	counter_temp += CLOCK_COUNTER_MAX - counter_tic;
	dtick = (unsigned int)(counter_temp % CLOCK_COUNTER_MAX);

	return dtick*get_tick_time();

}

unsigned int get_tick_counter(void){
	
	return counter;	

}

unsigned int get_tick_time(void){

	//return x ms per a tick
	unsigned long freq;

	freq = F_CPU;
    freq /= CLOCK_MAIN_PRESCALER;
    freq /=	CLOCK_COUNTER_TOP_VAL;
    freq /= CLOCK_SUB_PRESCALER;

	return 1000/freq;

}


ISR(CLOCK_TIMER_COMP_VECT){

	counter_sub = (counter_sub+1)%CLOCK_SUB_PRESCALER;

	if (counter_sub == 0){
	   
		counter = (counter + 1)%CLOCK_COUNTER_MAX;

		if (timer0_set_time_tick > 0){
			timer0_set_time_tick--;
			if (timer0_set_time_tick <= 0){
				timer0_set_time_tick = 0;
				fct_timer0_callback();
			}	

		}
		
		if (timer_btn_set_time_tick > 0){
			timer_btn_set_time_tick--;
			if (timer_btn_set_time_tick <= 0){
				timer_btn_set_time_tick = 0;
				fct_timer_btn_callback();
			}	

		}

	}

}



#include "clock.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define CLOCK_MAIN_PRESCALER 64

static unsigned int counter_sec = 0;
static unsigned int counter_100ms = 0;
static unsigned int counter = 0;

//counter for tic-toc, which saves counter value when tic() was called
static unsigned int counter_tic = 0;

//subcounter to generate 10ms
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
	OCR0A = CLOCK_HW_COUNTER_TOP_VAL; // output compare matchc occured in every 2ms
	TCCR0A |= (1<<WGM01); // prescale factor = 64, 8us per minor tick
	TCCR0B |= (1<<CS01)|(1<<CS00);	
	TIMSK |= (1<<OCIE0A);

#endif

#if defined(__AVR_ATmega8__)
	OCR2 = CLOCK_HW_COUNTER_TOP_VAL;
	TCCR2 |= (1<<WGM21)|(1<<CS22);
	TIMSK |= (1<<OCIE2);
		
#endif

#if defined(__AVR_ATmega88__)
	OCR0A = CLOCK_HW_COUNTER_TOP_VAL; // output compare matchc occured in every 2ms
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
	counter_temp += CLOCK_TICK_COUNTER_MAX - counter_tic;
	dtick = (unsigned int)(counter_temp % CLOCK_TICK_COUNTER_MAX);

	return dtick*get_tick_time();

}

unsigned int get_tick_counter_delta(void){

	int ret_val = 0;
	static int counter_old = 0;

	ret_val = (counter + CLOCK_TICK_COUNTER_MAX/2 - counter_old)%CLOCK_TICK_COUNTER_MAX - CLOCK_TICK_COUNTER_MAX/2 ;
	counter_old = counter;

	return ret_val;

}

unsigned int get_tick_counter(void){
	
	return counter;	

}

unsigned int get_100ms_counter(void){
	
	return counter_100ms;	

}

unsigned int get_tick_time(void){

	//return x ms per a tick
	unsigned long freq;

	freq = F_CPU;
    freq /= CLOCK_MAIN_PRESCALER;
    freq /=	CLOCK_HW_COUNTER_TOP_VAL;
    freq /= CLOCK_SUB_PRESCALER;

	return 1000/freq;

}


int pulse_generator_100ms_unit(int high_duration, int period, int count, unsigned char run){
	// return >0: pulse generator is running, remaining count value
	// return 0: stanby mode
	// run = 1: run pulse generator, 0: stop and reset pulse generator

	static int count_remain = 0;
	static unsigned char _state_cur = 0;
	static unsigned char _state_next = 0;

	static int counter_high, counter_low;


	_state_cur = _state_next;

	//basic initialization
	PULSE_PORT_DDR |= (1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB);

	
	switch(_state_cur){ //state 0:stanby, 1:high, 2:low
		case 1: //STATE_HIGH

			PULSE_PORT |= (1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB);

			counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;

			if (!run) _state_next = 0;
			else {
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_low)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = 2;

					counter_high = (counter_high + period) % CLOCK_100MS_COUNTER_MAX;
					count_remain--;
					if (count_remain <= 0) count_remain =0;
				}
			}
			break;
			
		case 2: //STATE_LOW

			PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));


			if ((!run) || (count_remain <= 0)) _state_next = 0;
			else{
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_high)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = 1;
					
					counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;

				}
			}
			break;

		default: //STATE_STANBY

			//clear both pulse ports
			PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));

			if(run) {
				_state_next = 1;

				count_remain = count;

				counter_high = get_100ms_counter();
				counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;
			}
	}

	// if (!state_cur) return 0;
	// else return count_remain;
	return _state_cur;

}


ISR(CLOCK_TIMER_COMP_VECT){

	//2ms loop
	counter_sub = (counter_sub+1)%CLOCK_SUB_PRESCALER;

	if (counter_sub == 0){
		//10ms loop
		counter = (counter + 1)%CLOCK_TICK_COUNTER_MAX;

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

		if (counter%(1000/get_tick_time()) == 1){
			//1sec loop
			counter_sec = (counter_sec+1)%CLOCK_SEC_COUNTER_MAX;
		}

		if (counter%(100/get_tick_time()) == 2){
			//100msec loop
			counter_100ms = (counter_100ms+1)%CLOCK_100MS_COUNTER_MAX;
		}

	}


}




#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "quad_encoder.h"
#include "clock.h"

static unsigned char flag_btn = 0;

static unsigned char flag_enc = 0;
static unsigned char pin_old = 0xff;

static unsigned char counter = 0;
static unsigned char status_btn_click = 0;
static unsigned char status_btn_hold = 0;

void timer_btn_timeout_handler(void){

	//timer_btn_install(timer_btn_timeout_handler, BTM_HOLD_TIMEOUT_MS);
	if (~(PC_PIN & (1<< PIN_BTN))){
		//if button is active at low
		if(flag_btn){
			status_btn_hold = 1;
			flag_btn = 0;
		}
	}	
}


unsigned char get_status_btn_click(void){

	unsigned char ret;
	ret = status_btn_click;
	status_btn_click = 0;

	return ret;
}

unsigned char get_status_btn_hold(void){

	unsigned char ret;
	ret = status_btn_hold;
	status_btn_hold = 0;

	return ret;
}
	

unsigned char get_enc_counter(void){

	return counter;

}


void init_encoder(void)
{
	_delay_ms(100);	

#if defined(__AVR_ATtiny85__)

	PCMSK |= (1<<PIN_BTN)|(1<<PIN_ENC_A)|(1<<PIN_ENC_B);
	GIMSK |= (1<<PCIE);

	pin_old = PC_PIN;
#endif

#if defined(__AVR_ATmega88__)
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PIN_BTN)|(1<<PIN_ENC_A)|(1<<PIN_ENC_B);

	pin_old = PC_PIN;

#if defined(__AVR_ATmega8__)
	
	MCUCR |= (1<<ISC00);
	GICR |= (1<<INT0);
	
	pin_old = PC_PIN;
#endif

}


#if defined(__AVR_ATtiny85__)
ISR(PCINT0_vect){
#endif

#if defined(__AVR_ATmega88__)
ISR(PCINT1_vect){
#endif
	
	unsigned char changed_bits, pin_read;
	char dx = 0;

	pin_read = PC_PIN;

	changed_bits = pin_read ^ pin_old;
	pin_old = pin_read;

	_delay_ms(1);
	pin_read = PC_PIN;

	if (changed_bits & (1<<PIN_BTN)) {
		
		_delay_ms(10);

		if (pin_read & (1<< PIN_BTN)) {
			//when the  button released
			if(flag_btn){
				if(is_timer_btn_alive()){
					kill_timer_btn();
					status_btn_click = 1;
				} else {
					;
				}
				flag_btn = 0;
			}
			
		} else {
			//when the button pressed
			flag_btn = 1;
			install_timer_btn(timer_btn_timeout_handler, BTN_HOLD_TIMEOUT_MS );
		}


	} 

	if (changed_bits & (1<< PIN_ENC_A)) {
		
		//detect falling edge
		//_delay_ms(1);
		//if(!(pinb & (1<<PIN_ENC_A))) flag_enc = 1;
		flag_enc = 2;
		//return;

	}

	if (changed_bits & (1<< PIN_ENC_B)) {

		if (flag_enc>0){

			flag_enc -= 1;
			
			if (pin_read & (1<<PIN_ENC_B)){
				//if rising edge
				dx = 1;	
			} else {
				//if falling edge
				dx = -1;
			}

			if (pin_read & (1<<PIN_ENC_A)){
				// High
				dx *= 1;
			} else {
				//Low
				dx *= -1;
			}

			counter += dx;

		}
	
	}
	
}
#endif

#if defined(__AVR_ATmega8__) 
ISR(INT0_vect){
	
	unsigned char changed_bits, pin_read;
	char dx = 0;

	pin_read = PC_PIN;

	changed_bits = pin_read ^ pin_old;
	pin_old = pin_read;

	_delay_ms(1);
	pin_read = PC_PIN;

	if (changed_bits & (1<<PIN_BTN)) {
		
		_delay_ms(10);

		if (pin_read & (1<< PIN_BTN)) {
			//when the  button released
			if(flag_btn){
				if(is_timer_btn_alive()){
					kill_timer_btn();
					status_btn_click = 1;
				} else {
					;
				}
				flag_btn = 0;
			}
			
		} else {
			//when the button pressed
			flag_btn = 1;
			install_timer_btn(timer_btn_timeout_handler, BTN_HOLD_TIMEOUT_MS );
		}


	} 

	if (changed_bits & (1<< PIN_ENC_A)) {
		
		//detect falling edge
		//_delay_ms(1);
		//if(!(pinb & (1<<PIN_ENC_A))) flag_enc = 1;
		flag_enc = 2;
		//return;

	}

	if (changed_bits & (1<< PIN_ENC_B)) {

		if (flag_enc>0){

			flag_enc -= 1;
			
			if (pin_read & (1<<PIN_ENC_B)){
				//if rising edge
				dx = 1;	
			} else {
				//if falling edge
				dx = -1;
			}

			if (pin_read & (1<<PIN_ENC_A)){
				// High
				dx *= 1;
			} else {
				//Low
				dx *= -1;
			}

			counter += dx;

		}
	
	}
	
}

#endif



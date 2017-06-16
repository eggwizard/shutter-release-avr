#include "pulse_generator.h"
#include "clock.h"


int pulse_gen_w_preamble_100ms_unit(int count, int high_duration, int period, int preamble, unsigned char cmd){
	// return >0: pulse generator is running, remaining count value
	// return 0: stanby mode

	static int count_remain = 0;
	static unsigned char _state_cur = 0;
	static unsigned char _state_next = 0;

	static int counter_high, counter_low, counter_preamble;
	// int preamble=0;

	// preamble = _preamble;
	if (period >= (POWER_SLEEP_SEC * 10)){
		preamble = WAKEUP_PREAMBLE_MS/100;
	}


	_state_cur = _state_next;

	//basic initialization
	PULSE_PORT_DDR |= (1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB);

	
	switch(_state_cur){ //state 0:stanby, 1:high, 2:low
		case PULSE_STATE_PREAMBLE:
			

			PULSE_PORT &= !(1<<PULSE_PIN_MAIN);
			PULSE_PORT |=(1<<PULSE_PIN_SUB);
			counter_high = (counter_preamble + preamble) % CLOCK_100MS_COUNTER_MAX;
			
			
			if (cmd == PULSE_GEN_CMD_RESET) {
				_state_next = PULSE_STATE_STANBY;
				PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));
			} else {
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_high)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = PULSE_STATE_HIGH;
			
				}
			}
			break;

		case PULSE_STATE_HIGH: //STATE_HIGH

			PULSE_PORT |= (1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB);

			counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;

			if (cmd == PULSE_GEN_CMD_RESET) {
				_state_next = PULSE_STATE_STANBY;
				PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));
			} else {
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_low)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = PULSE_STATE_LOW;

					counter_preamble = (counter_preamble + period) % CLOCK_100MS_COUNTER_MAX;
					count_remain--;
					if (count_remain <= 0) count_remain =0;
				}
			}
			break;
			
		case PULSE_STATE_LOW: //STATE_LOW

			PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));


			if ((cmd == PULSE_GEN_CMD_RESET) || (count_remain <= 0)){
				_state_next = PULSE_STATE_STANBY;
				PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));
			} else{
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_preamble)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = PULSE_STATE_PREAMBLE;
					
					counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;

				}
			}
			break;

		default: //STATE_STANBY

			//clear both pulse ports
			PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));

			if(cmd == PULSE_GEN_CMD_RUN) {
				_state_next = PULSE_STATE_PREAMBLE;

				count_remain = count;

				counter_preamble = get_100ms_counter();
				counter_high = (counter_preamble + preamble) % CLOCK_100MS_COUNTER_MAX;
				counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;
			}
	}

	// if (_state_cur == PULSE_STATE_STANBY) return 0;
	// else return count_remain;
	
	return count_remain;

	// return _state_cur;

}


int pulse_gen_100ms_unit(int count, int high_duration, int period, unsigned char cmd){
    // generate pulse with 100ms resolution
	// return >0: pulse generator is running, remaining count value
	// return 0: stanby mode

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

			if (cmd == PULSE_GEN_CMD_RESET) {
				_state_next = 0;
				PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));
			} else {
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


			if ((cmd == PULSE_GEN_CMD_RESET) || (count_remain <= 0)){
				_state_next = 0;
				PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));
			} else{
				if ((((int)get_100ms_counter() + CLOCK_100MS_COUNTER_MAX/2 - counter_high)%CLOCK_100MS_COUNTER_MAX - CLOCK_100MS_COUNTER_MAX/2) >= 0){
					_state_next = 1;
					
					counter_low = (counter_high + high_duration) % CLOCK_100MS_COUNTER_MAX;

				}
			}
			break;

		default: //STATE_STANBY

			//clear both pulse ports
			PULSE_PORT &= !((1<<PULSE_PIN_MAIN)|(1<<PULSE_PIN_SUB));

			if(cmd == PULSE_GEN_CMD_RUN) {
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

#define F_CPU 8000000UL


#include "quad_encoder.h"
#include "clock.c"
#include "oled.h"
#include "pulse_generator.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>


#ifndef MAX
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif


// unsigned char msg[20] = {0};

// unsigned char temp = 10;
// unsigned char temp1 = 0;
// int temp2 = 0;

#define MIN_SHUTTER_100MS	1
#define MIN_PERIOD_100MS	5
#define MIN_PREAMBLE_100MS	3

#define STATE_MAIN 0
#define STATE_SHUTTER_SPEED 1
#define STATE_PERIOD 2
#define STATE_NUM_SHOT 3
#define STATE_TIME_LAG 4
#define STATE_RUN 5


// unsigned int var_set_lagtime[5] = {0, 2, 5, 10};
unsigned int var_set_shutter[24] = {0,1,2,4,6,8, 10, 13,15, 20, 25, 30, 40, 60, 80, 100, 130, 150, 200, 250, 300, 400, 500, 500};
unsigned int var_set_period[16] = {0, 1, 2, 3, 4, 5, 8, 10, 20, 30, 50, 60, 100, 120, 180, 180};
unsigned int var_set_num[13] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 20, 20};

// char msg_lagtime[] 	= "  T_l:%4d sec  ";
char msg_shutter[] 	= "  T_s:%4d sec  ";
char msg_period[] 	= "  T_p:%4d sec  ";
char msg_num[] 		= "  N :%5d times";

typedef struct {
	unsigned int *var_set;
	int idx;
	unsigned int var_max;
	unsigned char len;
	unsigned char open_end;
	char *msg_format;
	unsigned char step;

} state_var_t;


// state_var_t state_var_lagtime = {
// 	var_set_lagtime, 0, 10, 4, 0, msg_lagtime, 0};

state_var_t state_var_shutter = {
	var_set_shutter, 0, 1200, 23, 1, msg_shutter, 10};

state_var_t state_var_period = {
	var_set_period, 1, 3600, 15, 1, msg_period, 10};

state_var_t state_var_num = {
	var_set_num, 2, 5000, 12, 1, msg_num, 5};



unsigned int state_cur = STATE_MAIN;
unsigned int state_var_menu = STATE_SHUTTER_SPEED;

#define state_var_menu_NUM 3

void sys_init(void)
{
	// _delay_ms(100);	
	init_encoder();	
	init_clock();
	init_oled();
	
	tic();
	sei();
}

void test_func (void){

	// char msg_temp[20]={0};

	// state_var_menu += get_enc_delta();
	// state_var_menu = ( state_var_menu +MENU_LV0_NUM -1 )%MENU_LV0_NUM + 1;

	// switch(state_var_menu){
	// 	case STATE_PERIOD:
	// 		sprintf(msg_temp, "<    Period    >");
	// 		break;
	// 	case STATE_TIME_LAG:
	// 		sprintf(msg_temp, "<   Lag Time   >");
	// 		break;
	// 	case STATE_NUM_SHOT:
	// 		sprintf(msg_temp, "< Num of Shots >");
	// 		break;
	// 	default:
	// 		sprintf(msg_temp, "< Shutter Time >");
	// 		break;
	// }
	// print_text(0, 0, msg_temp, 0);

	// sprintf(msg_temp, "BTN:%d%d, ENC:%3d", temp1, temp2, get_enc_counter());
	// print_text(0, 1, msg_temp, 0);
	
//	sprintf(msg_temp, "cur state:%d", state_cur );
//	print_text(0, 2, msg_temp, 0);
	
//	sprintf(msg_temp, "%d", state_var_menu);
//	print_text(0, 3, msg_temp, 0);

}


void set_state_var(state_var_t *state, int enc_delta){

	//if (state->var_max >= state->var_set[state->idx])
	if (state->var_set[(state->len)-1] >= state->var_set[state->idx])
		state->idx += enc_delta;
	
	if (state->idx <= 0) {
		state->idx = 0;
	} else if (state->idx >= state->len){
		if (state->open_end){
			state->idx = state->len;
			
			// state->var_set[state->idx] += (state->step)*enc_delta;
			// if (state->var_set[state->idx] >= state->var_max) state->var_set[state->idx] = state->var_max;
			
			state->var_set[state->len] += (state->step)*enc_delta;
			if (state->var_set[state->len] >= state->var_max) state->var_set[state->len] = state->var_max;

		} else {
			state->idx = state->len -1;
		}
	}

	
}


void state_num_job(void){

	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	set_state_var(&state_var_num, enc_input);
	
	sprintf(msg_temp2, state_var_num.msg_format, state_var_num.var_set[state_var_num.idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);
	
	if (btn_click_input) {
		state_cur = STATE_MAIN;
		clear_oled();
	}

}

void state_shutter_job(void){
	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();
	
	set_state_var(&state_var_shutter, enc_input);
	
	if (!state_var_shutter.var_set[state_var_shutter.idx]){
		sprintf(msg_temp2, "  T_s: A Pulse  ");
	} else {
		sprintf(msg_temp2, state_var_shutter.msg_format , state_var_shutter.var_set[state_var_shutter.idx]);
	}
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);
	
	if (btn_click_input) {
		state_cur = STATE_MAIN;
		clear_oled();
	}
}

void state_period_job(void){
	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	set_state_var(&state_var_period, enc_input);
	
	// sprintf(msg_temp2, state_var_period.msg_format, state_var_period.var_set[state_var_period.idx]);
	if (!state_var_period.var_set[state_var_period.idx]){
		sprintf(msg_temp2, "  T_p: 0.5 sec  ");
	} else {
		sprintf(msg_temp2, state_var_period.msg_format , state_var_period.var_set[state_var_period.idx]);
	}	
	
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);

	if (btn_click_input) {
		state_cur = STATE_MAIN;
		clear_oled();
	}
}

// void state_timelag_job(void){

// 	char msg_temp2[20] = {0};
// 	int enc_input, btn_click_input, btn_hold_input;
	
// 	enc_input = get_enc_delta();
// 	btn_click_input = get_status_btn_click();
// 	btn_hold_input = get_status_btn_hold();

// 	set_state_var(&state_var_lagtime, enc_input);
	
// 	sprintf(msg_temp2, state_var_lagtime.msg_format, state_var_lagtime.var_set[state_var_lagtime.idx]);
// 	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


// 	if (btn_click_input) {
// 		state_cur = STATE_MAIN;
// 		clear_oled();
// 	}
// }

void state_run_job(void){

	char msg_temp[17]={0};
	int cnt_remain;
	int enc_input, btn_click_input, btn_hold_input;
	
	// int shutter_val, period_val;

	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();
	
	// _temp = state_var_num.var_set[state_var_num.idx];
	// shudtter_val = state_var_shutter.var_set[state_var_shutter.idx];
	// period_val = state_var_period.var_set[state_var_period.idx];

	// _temp = pulse_gen_100ms_unit(010, 30, 3, NULL);
	// _temp = pulse_gen_w_preamble_100ms_unit(3,15,40,15,NULL);
	
	cnt_remain = pulse_gen_w_preamble_100ms_unit(
		state_var_num.var_set[state_var_num.idx],
		MAX(state_var_shutter.var_set[state_var_shutter.idx]*10, MIN_SHUTTER_100MS),
		MAX(state_var_period.var_set[state_var_period.idx]*10, MIN_PERIOD_100MS),
		MIN_PREAMBLE_100MS, NULL );

	// _temp = pulse_gen_w_preamble_100ms_unit(
	// 	state_var_num.var_set[state_var_num.idx],
	// 	(state_var_shutter.var_set[state_var_shutter.idx]),
	// 	(state_var_period.var_set[state_var_period.idx]),
	// 	3, NULL );
		
	sprintf(msg_temp, "T_p:%4ds", state_var_period.var_set[state_var_period.idx]);
	print_text(0, 0,msg_temp, NULL);

	// sprintf(msg_temp, "Ts:%3ds Tp:%4ds", state_var_shutter.var_set[state_var_shutter.idx], state_var_period.var_set[state_var_period.idx]);
	// print_text(0, 0,msg_temp, NULL);

	sprintf(msg_temp, "Counts:%4d/%4d", state_var_num.var_set[state_var_num.idx]-cnt_remain+1, state_var_num.var_set[state_var_num.idx]);
	print_text(0, 2, msg_temp, NULL);
	
	if (btn_hold_input || (cnt_remain == 0)) {
		state_cur = STATE_MAIN;
		pulse_gen_w_preamble_100ms_unit(0,0,0,0, PULSE_GEN_CMD_RESET );
		clear_oled();
	}
}

void state_main_job(void){

	char msg_temp1[20], msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	state_var_menu += enc_input;
	state_var_menu = ( state_var_menu +state_var_menu_NUM -1 )%state_var_menu_NUM + 1;

	switch(state_var_menu){
		case STATE_PERIOD:
			sprintf(msg_temp1, "<    Period    >");

			// sprintf(msg_temp2, state_var_period.msg_format, state_var_period.var_set[state_var_period.idx]);
			if (!state_var_period.var_set[state_var_period.idx]){
				sprintf(msg_temp2, "  T_p: 0.5 sec  ");
			} else {
				sprintf(msg_temp2, state_var_period.msg_format , state_var_period.var_set[state_var_period.idx]);
			}
			break;

		// case STATE_TIME_LAG:
		// 	sprintf(msg_temp1, "<   Lag Time   >");
			
		// 	sprintf(msg_temp2, state_var_lagtime.msg_format, state_var_lagtime.var_set[state_var_lagtime.idx]);
		// 	break;

		case STATE_NUM_SHOT:
			sprintf(msg_temp1, "< Num of Shots >");
			
			sprintf(msg_temp2, state_var_num.msg_format, state_var_num.var_set[state_var_num.idx]);
			break;

		default:
			sprintf(msg_temp1, "< Shutter Time >");
				
			if (!state_var_shutter.var_set[state_var_shutter.idx]){
				sprintf(msg_temp2, "  T_s: A Pulse  ");
			} else {
				sprintf(msg_temp2, state_var_shutter.msg_format , state_var_shutter.var_set[state_var_shutter.idx]);
			}
			break;
	}
	print_text(0, 0, msg_temp1, NULL);
	print_text(0, 1, msg_temp2, NULL);

	if (btn_click_input) {
		state_cur = state_var_menu;
		clear_oled();

	} else if (btn_hold_input) {
		if ((state_var_period.var_set[state_var_period.idx] > state_var_shutter.var_set[state_var_shutter.idx])
			|| ( !state_var_period.var_set[state_var_period.idx] && !state_var_shutter.var_set[state_var_shutter.idx] )){
			state_cur = STATE_RUN;
			// pulse_gen_100ms_unit(10, 30, 3, PULSE_GEN_CMD_RUN);
			// pulse_gen_w_preamble_100ms_unit(3, 15,40,15,PULSE_GEN_CMD_RUN);
			pulse_gen_w_preamble_100ms_unit(
				state_var_num.var_set[state_var_num.idx],
				MAX(state_var_shutter.var_set[state_var_shutter.idx]*10, MIN_SHUTTER_100MS),
				MAX(state_var_period.var_set[state_var_period.idx]*10, MIN_PERIOD_100MS),
				MIN_PREAMBLE_100MS, PULSE_GEN_CMD_RUN );

			clear_oled();

		} 
		
	}
	
}

void state_common_job(void){
	char msg_temp[20] = {0};
	unsigned long duration = 0;
	unsigned int hour, min, sec = 0;


	duration = (long)state_var_num.var_set[state_var_num.idx];
	duration *= (long)state_var_period.var_set[state_var_period.idx];

	hour = duration/3600;
	min = (duration%3600)/60;	
	sec = (duration%3600)%60;
	if (hour>=100) hour = 99;


	if (state_cur != STATE_RUN){
		sprintf(msg_temp, "Duration%02d:%02d:%02d", hour, min, sec);
		print_text(0,3, msg_temp, 0);

		if ((state_var_period.var_set[state_var_period.idx] > state_var_shutter.var_set[state_var_shutter.idx])
			|| ( !state_var_period.var_set[state_var_period.idx] && !state_var_shutter.var_set[state_var_shutter.idx] )) 
			sprintf(msg_temp, "                ");
			
		else {
			sprintf(msg_temp, "! Check Period !");
		} 
			
		print_text(0,2, msg_temp, 0);

	} 

}

int main(void)
{
	sys_init();
	// install_draw_function(test_func);

	for(;;){ 
	   	
		state_common_job();
		switch(state_cur){

			case STATE_NUM_SHOT:
				state_num_job();
				break;

			case STATE_PERIOD:
				state_period_job();
				break;
			
			case STATE_SHUTTER_SPEED:
				state_shutter_job();
				break;
			
			// case STATE_TIME_LAG:
			// 	state_timelag_job();
			// 	break;
			
			case STATE_RUN:
				state_run_job();
				break;		
			
			default:
				state_main_job();
		}

		// render_oled();
		// _delay_ms(100);

	}

}


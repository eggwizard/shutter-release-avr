#define F_CPU 8000000UL


#include "quad_encoder.h"
#include "clock.c"
#include "oled.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>


unsigned char msg[20] = {0};

unsigned char temp = 10;
unsigned char temp1 = 0;
int temp2 = 0;



#define STATE_MAIN 0
#define STATE_SHUTTER_SPEED 1
#define STATE_TIME_LAG 2
#define STATE_PERIOD 3
#define STATE_NUM_SHOT 4
#define STATE_RUN 5


unsigned int var_set_lagtime[5] = {0, 2, 5, 10};
unsigned int var_set_shutter[25] = {1,2,4,6,8, 10, 13,15, 20, 25, 30, 40, 50, 60, 80, 100, 130, 150, 200, 250, 300, 400, 500, 600, 610};
unsigned int var_set_period[15] = {1, 2, 4, 5, 8, 10, 20, 30, 50, 60, 100, 120, 180, 200, 210};
unsigned int var_set_num[13] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 20, 22};

char msg_lagtime[18] 	= "  T_l:%4d sec  ";
char msg_shutter[18] 	= "  T_s:%4d sec  ";
char msg_period[18] 	= "  T_p:%4d sec  ";
char msg_num[18] 		= "  N :%5d times";

typedef struct {
	unsigned int *var_set;
	int idx;
	unsigned int var_max;
	unsigned int len;
	unsigned char open_end;
	char *msg_format;
	int step;

} state_var_t;


state_var_t state_var_lagtime = {
	var_set_lagtime, 0, 10, 4, 0, msg_lagtime, 0};

state_var_t state_var_shutter = {
	var_set_shutter, 1, 600, 24, 1, msg_shutter, 10};

state_var_t state_var_period = {
	var_set_period, 1, 200, 14, 1, msg_period, 10};

state_var_t state_var_num = {
	var_set_num, 1, 20, 12, 1, msg_num, 5};



unsigned int state_cur = STATE_MAIN;
unsigned int state_var_menu = STATE_SHUTTER_SPEED;

#define state_var_menu_NUM 4

void sys_init(void)
{
	_delay_ms(100);	
	init_encoder();	
	init_clock();
	init_oled();
	
	tic();
	sei();
}

void test_func (void){

	char msg_temp[20]={0};

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
	
	sprintf(msg_temp, "cur state:%d", state_cur );
	print_text(0, 2, msg_temp, 0);
	
	sprintf(msg_temp, "%d", state_var_menu);
	print_text(0, 3, msg_temp, 0);

}


void set_state_var(state_var_t *state, int enc_delta){

	if (state->var_max >= state->var_set[state->idx])
		state->idx += enc_delta;
	
	if (state->idx <= 0) state->idx = 0;
	else if (state->idx >= state->len){
		if (state->open_end){
			state->idx = state->len;
			
			state->var_set[state->idx] += (state->step)*enc_delta;
						


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
	
	if (btn_click_input) state_cur = STATE_MAIN;

}

void state_shutter_job(void){
	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();
	
	set_state_var(&state_var_shutter, enc_input);
	
	sprintf(msg_temp2, state_var_shutter.msg_format , state_var_shutter.var_set[state_var_shutter.idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);

	if (btn_click_input) state_cur = STATE_MAIN;
}

void state_period_job(void){
	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	set_state_var(&state_var_period, enc_input);
	
	sprintf(msg_temp2, state_var_period.msg_format, state_var_period.var_set[state_var_period.idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


	if (btn_click_input) state_cur = STATE_MAIN;
}

void state_timelag_job(void){

	char msg_temp2[20] = {0};
	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	set_state_var(&state_var_lagtime, enc_input);
	
	sprintf(msg_temp2, state_var_lagtime.msg_format, state_var_lagtime.var_set[state_var_lagtime.idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


	if (btn_click_input) state_cur = STATE_MAIN;
}

void state_run_job(void){

	int enc_input, btn_click_input, btn_hold_input;
	
	enc_input = get_enc_delta();
	btn_click_input = get_status_btn_click();
	btn_hold_input = get_status_btn_hold();

	
	
	if (btn_hold_input) state_cur = STATE_MAIN;
}

state_main_job(){

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
			sprintf(msg_temp2, state_var_period.msg_format, state_var_period.var_set[state_var_period.idx]);
			break;
		case STATE_TIME_LAG:
			sprintf(msg_temp1, "<   Lag Time   >");
			sprintf(msg_temp2, state_var_lagtime.msg_format, state_var_lagtime.var_set[state_var_lagtime.idx]);
			break;
		case STATE_NUM_SHOT:
			sprintf(msg_temp1, "< Num of Shots >");
			sprintf(msg_temp2, state_var_num.msg_format, state_var_num.var_set[state_var_num.idx]);
			break;
		default:
			sprintf(msg_temp1, "< Shutter Time >");
			sprintf(msg_temp2, state_var_shutter.msg_format, state_var_shutter.var_set[state_var_shutter.idx]);
			break;
	}
	print_text(0, 0, msg_temp1, 0);
	print_text(0, 1, msg_temp2, 0);


	if (btn_click_input) {
		state_cur = state_var_menu;
	} else if (btn_hold_input) {
		state_cur = STATE_RUN;
	}

}


int main(void)
{
	sys_init();
	install_draw_function(test_func);

	for(;;){  

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
			
			case STATE_TIME_LAG:
				state_timelag_job();
				break;
			
			case STATE_RUN:
				state_run_job();
				break;		
			
			default:
				state_main_job();
		}

		render_oled();

	}

}


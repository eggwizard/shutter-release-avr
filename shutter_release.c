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


typedef struct {
	unsigned int var_set[5];
	int var_idx;
	unsigned int var_min;
	unsigned int var_max;
	unsigned int var_len;
	unsigned char var_open_end;

} state_var_lagtime_t;

state_var_lagtime_t state_var_lagtime = {
	{0, 2, 5, 10}, 0, 0, 10, 4, 0
};


typedef struct {
	unsigned int var_set[25];
	int var_idx;
	unsigned int var_min;
	unsigned int var_max;
	unsigned int var_len;
	unsigned char var_open_end;

} state_var_shutter_t;

state_var_shutter_t state_var_shutter = {
	{1,2,4,6,8, 10, 13,15, 20, 25, 30, 40, 50, 60, 80, 100, 130, 150, 200, 250, 300, 400, 500, 600}, 0, 1, 600, 24, 1
};


typedef struct {
	unsigned int var_set[15];
	int var_idx;
	unsigned int var_min;
	unsigned int var_max;
	unsigned int var_len;
	unsigned char var_open_end;

} state_var_period_t;

state_var_period_t state_var_period = {
	{1, 2, 4, 5, 8, 10, 20, 30, 50, 60, 100, 120, 180, 200 }, 5, 1, 200, 14, 1
};


typedef struct {
	unsigned int var_set[13];
	int var_idx;
	unsigned int var_min;
	unsigned int var_max;
	unsigned int var_len;
	unsigned char var_open_end;

} state_var_num_t;

state_var_num_t state_var_num = {
	{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 20}, 0, 1, 20, 12, 1
};




unsigned int state_cur = STATE_MAIN;

// unsigned int state_var_num = 1;
// unsigned int state_var_shutter_speed = 0;
// unsigned int state_var_time_lag = 2;
// unsigned int state_var_period = 1;

unsigned int state_var_menu = STATE_SHUTTER_SPEED;
#define state_var_menu_NUM 4

// unsigned int lag_time_idx = 0;
// unsigned int shutter_time_idx = 0;
// unsigned int period_idx = 0;
// unsigned int num_shot_idx = 0;




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


state_num_job(){
	char msg_temp2[20] = {0};

	state_var_num.var_idx += get_enc_delta();
	
	if (state_var_num.var_idx <= 0) state_var_num.var_idx = 0;
	else if (state_var_num.var_idx >= state_var_num.var_len){
		state_var_num.var_idx = state_var_num.var_len -1;
	}

	sprintf(msg_temp2, "   :%6d", state_var_num.var_set[state_var_num.var_idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


}

state_shutter_job(){
	char msg_temp2[20] = {0};

	state_var_shutter.var_idx += get_enc_delta();
	
	if (state_var_shutter.var_idx <= 0) state_var_shutter.var_idx = 0;
	else if (state_var_shutter.var_idx >= state_var_shutter.var_len){
		state_var_shutter.var_idx = state_var_shutter.var_len -1;
	}

	sprintf(msg_temp2, "   :%6d", state_var_shutter.var_set[state_var_shutter.var_idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);

}

state_period_job(){
	char msg_temp2[20] = {0};

	state_var_period.var_idx += get_enc_delta();
	
	if (state_var_period.var_idx <= 0) state_var_period.var_idx = 0;
	else if (state_var_period.var_idx >= state_var_period.var_len){
		state_var_period.var_idx = state_var_period.var_len -1;
	}

	sprintf(msg_temp2, "   :%6d", state_var_period.var_set[state_var_period.var_idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


}

state_timelag_job(){
	char msg_temp2[20] = {0};

	state_var_lagtime.var_idx += get_enc_delta();
	
	if (state_var_lagtime.var_idx <= 0) state_var_lagtime.var_idx = 0;
	else if (state_var_lagtime.var_idx >= state_var_lagtime.var_len){
		state_var_lagtime.var_idx = state_var_lagtime.var_len -1;
	}

	sprintf(msg_temp2, "   :%6d", state_var_lagtime.var_set[state_var_lagtime.var_idx]);
	print_text(0, 1, msg_temp2, OLED_PRINT_BLINK);


}

state_run_job(){


}

state_main_job(){

	char msg_temp1[20], msg_temp2[20] = {0};

	state_var_menu += get_enc_delta();
	state_var_menu = ( state_var_menu +state_var_menu_NUM -1 )%state_var_menu_NUM + 1;

	switch(state_var_menu){
		case STATE_PERIOD:
			sprintf(msg_temp1, "<    Period    >");
			sprintf(msg_temp2, "   :%6d", state_var_period.var_set[state_var_period.var_idx]);
			break;
		case STATE_TIME_LAG:
			sprintf(msg_temp1, "<   Lag Time   >");
			sprintf(msg_temp2, "   :%6d", state_var_lagtime.var_set[state_var_lagtime.var_idx]);
			break;
		case STATE_NUM_SHOT:
			sprintf(msg_temp1, "< Num of Shots >");
			sprintf(msg_temp2, "   :%6d", state_var_num.var_set[state_var_num.var_idx]);
			break;
		default:
			sprintf(msg_temp1, "< Shutter Time >");
			sprintf(msg_temp2, "   :%6d", state_var_shutter.var_set[state_var_shutter.var_idx]);
			break;
	}
	print_text(0, 0, msg_temp1, 0);
	print_text(0, 1, msg_temp2, 0);

}


int main(void)
{
	sys_init();
	install_draw_function(test_func);

	for(;;){  

		switch(state_cur){

			case STATE_NUM_SHOT:
				state_num_job();
				if (get_status_btn_click()) state_cur = STATE_MAIN;
				get_status_btn_hold();
				
				break;

			case STATE_PERIOD:
				state_period_job();
				if (get_status_btn_click()) state_cur = STATE_MAIN;
				get_status_btn_hold();
				//get_enc_delta();
				break;
			
			case STATE_SHUTTER_SPEED:
				state_shutter_job();
				if (get_status_btn_click()) state_cur = STATE_MAIN;
				get_status_btn_hold();
				//get_enc_delta();
				break;
			
			case STATE_TIME_LAG:
				state_timelag_job();
				if (get_status_btn_click()) state_cur = STATE_MAIN;
				get_status_btn_hold();
				//get_enc_delta();
				break;
			
			case STATE_RUN:
				state_run_job();
				if (get_status_btn_hold()) state_cur = STATE_MAIN;
				get_status_btn_hold();
				get_enc_delta();
				break;		
			
			default:
				state_main_job();
				if (get_status_btn_click()) {
					state_cur = state_var_menu;
				} else if (get_status_btn_hold()) {
					state_cur = STATE_RUN;
				}
		}

		render_oled();

	}

}


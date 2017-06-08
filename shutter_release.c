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
unsigned char temp2 = 0;



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

	sprintf(msg_temp, "< Shutter > ");
	print_text(0, 10, msg_temp, OLED_PRINT_BLINK);

	sprintf(msg_temp, "BTN:%d%d, ENC:%3d", temp1, temp2, get_enc_counter());
	print_text(0, 21, msg_temp, 0);
	
	//sprintf(msg_temp, "PINC012 %d%d%d", (PINC&1),(PINC&2)>0, (PINC&4)>0 );
	//print_text(0, 32, msg_temp, 0);
	
	//sprintf(msg_temp, "INT0(PD2) %d", (PIND & 0b100)>0);
	//print_text(0, 3, msg_temp, 0);
}


int main(void)
{
	sys_init();

	//install_timer0(test_func, 2000);
	//sprintf(msg, "timer installed");

	install_draw_function(test_func);

	for(;;){  

//		u8g2_FirstPage(&u8g2);
//		do{
//	  		draw();
//
//		} while ( u8g2_NextPage(&u8g2) );
		
		//sprintf(msg, "val:%d",temp);
		temp1 ^= get_status_btn_click();
		temp2 ^= get_status_btn_hold();
		//sprintf(msg, "BTN:%d%d, ENC:%3d", temp1, temp2, get_enc_counter());
		//sprintf(msg, "val:%03d", get_enc_counter());	
		
		//print_text(0, 15, msg, 0);

		render_oled();

	}

}


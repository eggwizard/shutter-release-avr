#define F_CPU 8000000UL

//#include "u8x8_avr_hal.h"
//#include "u8g2.h"
#include "quad_encoder.h"
#include "clock.c"

#include "oled.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>

//u8g2_t u8g2; // a structure which will contain all the data for one display

unsigned char msg[20] = {0};
unsigned char temp = 10;

unsigned char temp1 = 0;
unsigned char temp2 = 0;


//void u8g_setup(void)
//{  
//	
//	u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_gpio_and_delay_avr);
//
//	//u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_avr);
//
//	u8g2_InitDisplay(&u8g2);
//	u8g2_ClearDisplay(&u8g2);
//	u8g2_SetPowerSave(&u8g2, 0);
//}


void sys_init(void)
{
	_delay_ms(100);	
	init_encoder();	
	init_clock();
	init_oled();
	
	tic();
	sei();
}

//
//void draw(void)
//{
//	//u8g2_SetFont(&u8g2, u8g2_font_pro)font12_mf);
//	u8g2_SetFont(&u8g2, u8g2_font_profont15_tr);
//	//u8g2_SetFont(&u8g2, u8g2_font_6x10_mr);
// 	
//	u8g2_DrawStr(&u8g2, 0, 14,"Hello world!");
//	//u8g2_DrawStr(&u8g2, 0,19,"My name is eggwizard");
//	u8g2_DrawStr(&u8g2, 0, 29, msg);
//
//	//val = ((unsigned char)(val+10)%10); 
//	//u8g2_DrawGlyph(&u8g2, 0,30, val+ '0');
//	//u8g2_DrawStr(&u8g2, 0,32,"u8g2 lib.");
//
//
//}

void test_func (void){

	//sprintf(msg, "Test triggerd");
	//temp = 100;
	//
	char msg_temp[20]={0};

	sprintf(msg_temp, "BTN:%d%d, ENC:%3d", temp1, temp2, get_enc_counter());
	print_text(0, 0, msg_temp, 0);
	
	sprintf(msg_temp, "PINC012 %d%d%d", (PINC&1),(PINC&2)>0, (PINC&4)>0 );
	print_text(0, 1, msg_temp, 0);
	
	sprintf(msg_temp, "INT0(PD2) %d", (PIND & 0b100)>0);
	print_text(0, 2, msg_temp, 0);
}


int main(void)
{
	sys_init();
//	u8g_setup();

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


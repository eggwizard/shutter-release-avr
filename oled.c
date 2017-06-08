

#include "oled.h"
#include "u8x8_avr_hal.h"
#include "clock.h"

#if OLED_USE_COMPACT_LIBRARY

#include "u8x8.h"
static u8x8_t u8x8;

#else

#include "u8g2.h"
static u8g2_t u8g2; // a structure which will contain all the data for one display

#endif

static void (*draw_fct)(void);

static unsigned int blink_timestamp_ms = 0;
static unsigned char blink_flag = 0;

void install_draw_function(void (*fct)(void)){
	draw_fct = fct;

}

void init_oled(void){

#if OLED_USE_COMPACT_LIBRARY

	//u8x8_Setup(&u8x8, u8x8_d_ssd1306_128x32_univision, u8x8_cad_ssd13xx_i2c, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_avr);	
	u8x8_Setup(&u8x8, u8x8_d_ssd1306_128x32_univision, u8x8_cad_ssd13xx_i2c, u8x8_byte_avr_hw_i2c, u8x8_gpio_and_delay_avr);
	
	u8x8_InitDisplay(&u8x8);
	u8x8_ClearDisplay(&u8x8);
	u8x8_SetPowerSave(&u8x8, 0);	

	u8x8_SetFont(&u8x8, OLED_FONT);

#else

	u8g2_Setup_ssd1306_i2c_128x32_univision_2(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_gpio_and_delay_avr);
	//u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_gpio_and_delay_avr);
	//u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_avr);

	u8g2_InitDisplay(&u8g2);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_SetFont(&u8g2, OLED_FONT);

#endif


}



void render_oled(void){

#if OLED_USE_COMPACT_LIBRARY
	draw_fct();
#else
	u8g2_FirstPage(&u8g2);
	do{	draw_fct(); } while ( u8g2_NextPage(&u8g2) );
#endif

	
}

int get_value(){
	return blink_flag;
}


void print_text(int x, int y, char *msg, unsigned char flag){

	unsigned char offset = 0;

	if((get_tick_counter() + CLOCK_COUNTER_MAX - blink_timestamp_ms/get_tick_time())%CLOCK_COUNTER_MAX >= (OLED_BLINK_INTERVAL_MS/get_tick_time())){
		blink_timestamp_ms = get_tick_counter()*get_tick_time();
		blink_flag ^= (unsigned char)1;
	}


	if ((flag & OLED_PRINT_BLINK) && blink_flag){
		while(*(msg+offset) !=0){
			*(msg+offset)= ' ';
			offset++;
		}
	}


#if OLED_USE_COMPACT_LIBRARY
	if (flag & OLED_PRINT_INVERT) u8x8_SetInverseFont(&u8x8, 1);
	u8x8_DrawString(&u8x8, x, y, msg);
	if (flag & OLED_PRINT_INVERT) u8x8_SetInverseFont(&u8x8, 0);
#else
	if (flag & OLED_PRINT_INVERT) {u8g2_SetDrawColor(&u8g2, 0); u8g2_SetFontMode(&u8g2, 0);}
	u8g2_DrawStr(&u8g2, x, y, msg);
	if (flag & OLED_PRINT_INVERT) {u8g2_SetDrawColor(&u8g2, 1); u8g2_SetFontMode(&u8g2, 1);}
#endif


}



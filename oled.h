#ifndef _OLED_H_
#define _OLED_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif



#define OLED_USE_COMPACT_LIBRARY 1
#define OLED_BLINK_INTERVAL_MS 250

#define OLED_PRINT_INVERT 	1
#define OLED_PRINT_BLINK 	2


#if OLED_USE_COMPACT_LIBRARY

#define OLED_FONT u8x8_font_artossans8_r
//#define OLED_FONT u8x8_font_chroma48medium8_r
//#define OLED_FONT u8x8_font_victoriamedium8_r
//#define OLED_FONT u8x8_font_torussansbold8_r
//#define OLED_FONT u8x8_font_pressstart2p_r


#else

#define OLED_FONT u8g2_font_profont15_tr
//#define OLED_FONT u8g2_font_6x10_mri

#endif

void install_draw_function(void (*fct)(void));
void init_oled(void);
void render_oled(void);
void print_text(int x, int y, char *msg, unsigned char);
void clear_oled(void);

/*
[Draw function example]

void draw_func_template (void){
	char msg_temp[20]={0};
	sprintf(msg_temp, "Hello world!");
	print_text(0, 15, msg_temp, 0);
}
...
install_draw_function(test_func);
*/



#endif

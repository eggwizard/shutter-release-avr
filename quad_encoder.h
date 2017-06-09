#ifndef _QUAD_ENCODER_H_
#define _QUAD_ENCODER_H_


#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
#define PC_PIN PINC
#define PIN_BTN 2
#define PIN_ENC_A 0//4
#define PIN_ENC_B 1//3
#endif

#if defined(__AVR_ATtiny85__)
#define PC_PIN PINC
#define PIN_BTN 2
#define PIN_ENC_A 0//4
#define PIN_ENC_B 1//3
#endif


#define BTN_HOLD_TIMEOUT_MS 800

void timer_btn_timeout_handler(void);
unsigned char get_status_btn_click(void);
unsigned char get_status_btn_hold(void);

void init_encoder(void);
unsigned char get_enc_counter(void);
int get_enc_delta(void);


#endif

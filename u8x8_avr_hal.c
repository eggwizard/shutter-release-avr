#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "u8x8_avr_hal.h"



#if defined(__AVR_ATtiny85__)
#include "TinyWireM_USI.h"
#define I2C_PORT PORTB
#define I2C_PORT_DIR DDRB
#define I2C_CLOCK_PORT 2
#define I2C_DATA_PORT 0
#endif

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)

#define I2C_PORT PORTC
#define I2C_PORT_DIR DDRC
#define I2C_CLOCK_PORT 5
#define I2C_DATA_PORT 4

#endif

#if defined(__AVR_ATmega88__)

// blah blah

#endif


unsigned char u8x8_byte_avr_hw_i2c(u8x8_t *u8x8, unsigned char msg, unsigned char arg_int, void *arg_ptr){

	
#if defined(__AVR_ATmega8__)||defined(__AVR_ATmega88__)	
	
	unsigned char *data;
	unsigned char internal_i2c_mode;

	switch(msg){
		case U8X8_MSG_BYTE_SEND:
			
				  
			data = (uint8_t *)arg_ptr;
			while( arg_int > 0 ) {
				TWDR = (unsigned char)*data;
				TWCR = ((1<< TWINT) | (1<<TWEN));
				while (!(TWCR & (1 <<TWINT)));
				data++;
				arg_int--;
			}  
		
			break;

		case U8X8_MSG_BYTE_INIT:
			//set clock, set data and delay
			//initialize twi module

			TWSR=0x00; //set presca1er bits to zero
			//TWBR=0x46; //SCL frequency is 100K for XTAL = 7.3728M
			//TWBR=2; //SCL frequency is 100K for XTAL = 7.3728M
			TWBR=3;
			TWCR=0x04; //enab1e TWI module
			
			break;

		case U8X8_MSG_BYTE_SET_DC:
			//nop
			break;

		case U8X8_MSG_BYTE_START_TRANSFER:
			//start i2c
			//issue address

			TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
			while (!(TWCR & (1<<TWINT)));
			
			TWDR = u8x8_GetI2CAddress(u8x8);
			TWCR = ((1<< TWINT) | (1<<TWEN));
			while (!(TWCR & (1 <<TWINT)));

			break;
		
		
		case U8X8_MSG_BYTE_END_TRANSFER:
			//stop i2c
			TWCR = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
			_delay_us(10) ; //wait for a short time
			
			break;

		default:
			return 0;

	}

	return 1;


#endif

#if defined(__AVR_ATtiny85__)


	
	unsigned char *data;
	unsigned char internal_i2c_mode;

	switch(msg){
		case U8X8_MSG_BYTE_SEND:
			
				  
			data = (uint8_t *)arg_ptr;
			while( arg_int > 0 ) {
				
				i2c_write( (unsigned char)*data );

				data++;
				arg_int--;
			}  
		
			break;

		case U8X8_MSG_BYTE_INIT:
			//set clock, set data and delay
			//initialize twi module

			i2c_init();

			break;

		case U8X8_MSG_BYTE_SET_DC:
			//nop
			break;

		case U8X8_MSG_BYTE_START_TRANSFER:
			//start i2c
			//issue address

//			TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
//			while (!(TWCR & (1<<TWINT)));
//			
//			TWDR = u8x8_GetI2CAddress(u8x8);
//			TWCR = ((1<< TWINT) | (1<<TWEN));
//			while (!(TWCR & (1 <<TWINT)));

			i2c_start(u8x8_GetI2CAddress(u8x8));

			break;
		
		
		case U8X8_MSG_BYTE_END_TRANSFER:
			//stop i2c
			
			i2c_stop();

			break;

		default:
			return 0;

	}

	return 1;




#endif

}





unsigned char u8x8_gpio_and_delay_avr(u8x8_t *u8x8, unsigned char msg, unsigned char arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            /* only support for software I2C*/
            // Chip_IOCON_PinMuxSet(LPC_IOCON, I2C_CLOCK_PORT, I2C_CLOCK_PIN, IOCON_MODE_PULLUP);
            // Chip_IOCON_PinMuxSet(LPC_IOCON, I2C_DATA_PORT, I2C_DATA_PIN, IOCON_MODE_PULLUP);
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, I2C_CLOCK_PORT, I2C_CLOCK_PIN);
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, I2C_DATA_PORT, I2C_DATA_PIN);    

            // Chip_IOCON_PinMuxSet(LPC_IOCON, KEY_SELECT_PORT, KEY_SELECT_PIN, KEY_SELECT_FN|IOCON_MODE_PULLUP);    
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_SELECT_PORT, KEY_SELECT_PIN);

            // Chip_IOCON_PinMuxSet(LPC_IOCON, KEY_PREV_PORT, KEY_PREV_PIN, IOCON_MODE_PULLUP);    
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_PREV_PORT, KEY_PREV_PIN);

            // Chip_IOCON_PinMuxSet(LPC_IOCON, KEY_NEXT_PORT, KEY_NEXT_PIN, IOCON_MODE_PULLUP);    
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_NEXT_PORT, KEY_NEXT_PIN);

            // Chip_IOCON_PinMuxSet(LPC_IOCON, KEY_HOME_PORT, KEY_HOME_PIN, KEY_HOME_FN|IOCON_MODE_PULLUP);    
            // Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_HOME_PORT, KEY_HOME_PIN);

            I2C_PORT_DIR &= ~((1<<I2C_CLOCK_PORT)|(1<<I2C_DATA_PORT));

            break;
        case U8X8_MSG_DELAY_NANO:
            /* not required for SW I2C */
            break;

        case U8X8_MSG_DELAY_10MICRO:
            /* not used at the moment */
            break;

        case U8X8_MSG_DELAY_100NANO:
            /* not used at the moment */
            break;

        case U8X8_MSG_DELAY_MILLI:
            _delay_us(1000);
            break;

        case U8X8_MSG_DELAY_I2C:
            /* arg_int is 1 or 4: 100KHz (5us) or 400KHz (1.25us) */
            //delay_micro_seconds(arg_int<=2?5:1);
            _delay_us(1);
            break;

        case U8X8_MSG_GPIO_I2C_CLOCK:
            if ( arg_int == 0 )
            {
                //Chip_GPIO_SetPinDIROutput(LPC_GPIO, I2C_CLOCK_PORT, I2C_CLOCK_PIN);
                //Chip_GPIO_SetPinOutLow(LPC_GPIO, I2C_CLOCK_PORT, I2C_CLOCK_PIN);
                I2C_PORT_DIR |= (1<<I2C_CLOCK_PORT);
                I2C_PORT &= ~(1<<I2C_CLOCK_PORT);
                
            }
            else
            {
                ////Chip_GPIO_SetPinOutHigh(LPC_GPIO, I2C_CLOCK_PORT, I2C_CLOCK_PIN);
                //Chip_GPIO_SetPinDIRInput(LPC_GPIO, I2C_CLOCK_PORT, I2C_CLOCK_PIN);
                I2C_PORT_DIR &= ~(1<<I2C_CLOCK_PORT);

            }
            break;

        case U8X8_MSG_GPIO_I2C_DATA:
            if ( arg_int == 0 )
            {
                //Chip_GPIO_SetPinDIROutput(LPC_GPIO, I2C_DATA_PORT, I2C_DATA_PIN);
                //Chip_GPIO_SetPinOutLow(LPC_GPIO, I2C_DATA_PORT, I2C_DATA_PIN);
                I2C_PORT_DIR |= (1<<I2C_DATA_PORT);
                I2C_PORT &= ~(1<<I2C_DATA_PORT);
            }
            else
            {
                // //Chip_GPIO_SetPinOutHigh(LPC_GPIO, I2C_DATA_PORT, I2C_DATA_PIN);
                // Chip_GPIO_SetPinDIRInput(LPC_GPIO, I2C_DATA_PORT, I2C_DATA_PIN);
                I2C_PORT_DIR &= ~(1<<I2C_DATA_PORT);
            }
            break;

//        case U8X8_MSG_GPIO_MENU_SELECT:
//            // u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_SELECT_PORT, KEY_SELECT_PIN));
//            break;
//
//        case U8X8_MSG_GPIO_MENU_NEXT:
//            // u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_NEXT_PORT, KEY_NEXT_PIN));
//            break;
//
//        case U8X8_MSG_GPIO_MENU_PREV:
//            // u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_PREV_PORT, KEY_PREV_PIN));
//            break;
//
//
//        case U8X8_MSG_GPIO_MENU_HOME:
//            // u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_HOME_PORT, KEY_HOME_PIN));
//            break;

        default:
            // u8x8_SetGPIOResult(u8x8, 1);
            break;
    }
    return 1;
}

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU 
#define F_CPU 16000000UL
#endif 

#define FOSC 16000000 // Uart Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1 

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lora_api.h"
#include "spi_comm.h"
#include "uart_api.h"
#include <string.h>


unsigned char *tx_data = "Here";

/* Reset pin = 9 - PB1
   DIO pin = 2 - PD2*/

int main()
{
    int ret;
    enum sx127x_err ret_sx127x;
    sx127x_dev dev;
    int i;
    /* Function assignments */
    dev.spi_read = spi_read_hl;
    dev.spi_write = spi_write_hl;
    //dev.delay_ms =  _delay_ms;
    dev.bw = 125e3; // Default
    dev.sf = 7; // Default
    dev.txpow = 17; 
    dev.freq = 868e6; //European ISM band

    /* Reset the module */
    DDRB |= 1 << DDB1;
    PORTB |= 1 <<  PORT1;
    _delay_ms(15);
    PORTB &=~ (1 << PORT1);
    _delay_ms(50);
    PORTB |= (1 << PORT1);

    /* DIO0 pin */
    // Pull down or Pull up ?
    PORTD |= 1 << PORT2;
    DDRD |= 0 << PORT2;

    /* SPI Init */
    spi_master_init();

    /* UART INIT */
    uart_init(MYUBRR);
    
    /* LoRa Init */
    ret_sx127x = sx127x_lora_init(&dev);
    if(ret_sx127x < 0) {
        return ret_sx127x;
    }

    /* Endless loop */
    while(1){
        /* Send the data every 10 seconds */
        ret_sx127x = sx127x_lora_tx_data(&dev, tx_data, strlen(tx_data));
        if(ret_sx127x < 0) {
            return ret_sx127x;
        }
        for(i = 0; i < 150 ; i++) {
            _delay_ms(10);
        }
    }
    return 0;
}

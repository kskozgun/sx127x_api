#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU 
#define F_CPU 1E6UL
#endif 

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lora_api.h"
#include "spi_comm.h"
#include <string.h>

/* Reset pin = 9 - PB1
   DIO pin = 2 - PD2*/

int main()
{
    int ret;
    enum sx127x_err ret_sx127x;
    sx127x_dev dev;
    unsigned char *tx_data = "Hello";
    int i;
    /* Function assignments */
    dev.spi_read = spi_read_hl;
    dev.spi_write = spi_write_hl;
    dev.delay_ms =  _delay_ms;

    /* Reset the module */
    DDRB = 1 << PORT1;
    PORTB |= 1 <<  PORT1;
    dev.delay_ms(15);
    PORTB &=~ (1 << PORT1);
    dev.delay_ms(50);
    PORTB |= (1 << PORT1);

    /* DIO0 pin */
    // Pull down or Pull up ?
    DDRD = 0 << PORT2;

    /* SPI Init */
    spi_master_init();

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
        for(i = 0; i < 50 ; i++) {
            dev.delay_ms(10);
        }
    }
    return 0;
}

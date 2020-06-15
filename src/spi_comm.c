#include "spi_comm.h"

void spi_master_init(void)
{
    /* SS -> PB2 , MOSI -> PB3, SCK -> PB5 = OUTPUT */
    DDRB |= (1<<DDB2) | (1<<DDB3) | (1<<DDB5);
    /* Enable SPI, Master Mode, FSK/16 */
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void spi_read(uint8_t reg, uint8_t *rx_buffer)
{
    /* SS LOW */
    PORTB &= ~(1 << (PB2));
    SPDR = reg;
    while(!(SPSR & (1<<SPIF)));
    /* Send Dummy Data */
    SPDR = 0xFF;
    while(!(SPSR & (1<<SPIF)));
    *rx_buffer = SPDR;
    /* SS HIGH */
    PORTB |= (1 << (PB2));
}

void spi_write(uint8_t reg, uint8_t *tx_data)
{
    uint8_t reg_temp;
    reg_temp = reg | (0x80);
    PORTB &= ~(1 << (PB2));  
    SPDR = reg_temp;
    while(!(SPSR & (1<<SPIF)));
    SPDR = *tx_data;
    while(!(SPSR & (1<<SPIF)));
    PORTB |= (1 << (PB2));
}

int8_t spi_read_hl(uint8_t reg, uint8_t *rx_buffer, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < len; i++){
        spi_read(reg, rx_buffer);
        reg++;
        rx_buffer++;
    }
    return 0;
}

int8_t spi_write_hl(uint8_t reg, uint8_t *tx_data, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < len; i++){
        spi_write(reg, tx_data);
        reg++;
        tx_data++;
    }
    return 0;
}

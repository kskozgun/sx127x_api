#ifndef _SPI_COMM_H_
#define _SPI_COMM_H_

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdint.h>
#include <avr/io.h>

void spi_master_init(void);

void spi_read(uint8_t reg, uint8_t *rx_buffer);

int8_t spi_read_hl(uint8_t reg, uint8_t *rx_buffer, uint8_t len);

void spi_write(uint8_t reg, uint8_t *tx_data);

int8_t spi_write_hl(uint8_t reg, uint8_t *tx_data, uint8_t len);

#endif //_SPI_COMM_H_
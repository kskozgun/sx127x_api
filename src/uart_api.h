#ifndef _UART_COMM_H_
#define _UART_COMM_H_

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

void uart_init(int baudrate);

void uart_transmit(unsigned char *data, uint8_t size);

unsigned char uart_receive();


#endif //_UART_COMM_H_
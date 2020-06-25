#include "uart_api.h"

void uart_init(int baudrate)
{
     /*Set baud rate */
     UBRR0H = (unsigned char)(baudrate>>8);
     UBRR0L = (unsigned char)baudrate;
     /*Enable receiver and transmitter */
     UCSR0B = (1<<RXEN0)|(1<<TXEN0);
     /* Set frame format: 8data, 1stop bit */
     UCSR0C = (3<<UCSZ00); 
}

void uart_transmit(unsigned char data)
{
        /* Wait for empty transmit buffer */
        while (!(UCSR0A & (1<<UDRE0)));
        /* Put data into buffer, sends the data */
        UDR0 = data;
}

void uart_transmit_hl(unsigned char *data, uint8_t size)
{
    int i;
    for(i = 0; i < size; i++) {
        /* Wait for empty transmit buffer */
        while (!(UCSR0A & (1<<UDRE0)));
        /* Put data into buffer, sends the data */
        UDR0 = *data;
        data++;
    }
}

unsigned char uart_receive()
{
    /* Wait for data to be received */
    while (!(UCSR0A & (1<<RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}

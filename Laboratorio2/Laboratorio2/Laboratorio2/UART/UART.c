/*
 * UART.c
 *
 * Created: 26/01/2026 09:55:01
 *  Author: samur
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include "UART.h"

#define BAUD 9600
#define UBRR_VAL ((F_CPU / 16 / BAUD) - 1)
char uart_rx;

void UART_Init(void)
{
	UBRR0H = (uint8_t)(UBRR_VAL >> 8);
	UBRR0L = (uint8_t)UBRR_VAL;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_SendChar(char c)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

void UART_SendString(const char *str)
{
	while (*str)
	UART_SendChar(*str++);
}

ISR(USART_RX_vect)
{
	uart_rx = UDR0;
}
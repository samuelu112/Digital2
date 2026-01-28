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
volatile char uart_rx;

void UART_Init(void)
{
	UBRR0H = (uint8_t)(UBRR_VAL >> 8);
	UBRR0L = (uint8_t)UBRR_VAL;
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
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

volatile char uart_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_buffer_index = 0;

ISR(USART_RX_vect)
{
	char c = UDR0;
	if (uart_buffer_index < UART_BUFFER_SIZE - 1) {
		uart_buffer[uart_buffer_index++] = c;
	}
}

char UART_GetChar(void)
{
	char c = 0;
	if (uart_buffer_index > 0) {
		c = uart_buffer[0];
		// Desplazar el buffer
		for (uint8_t i = 1; i < uart_buffer_index; i++) {
			uart_buffer[i-1] = uart_buffer[i];
		}
		uart_buffer_index--;
	}
	return c;
}
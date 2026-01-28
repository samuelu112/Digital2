/*
 * Laboratorio2.c
 *
 * Created: 22/01/2026 19:14:28
 * Author : samur
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD/lcd.h"
#include "UART/UART.h"

uint16_t leerADC(uint8_t canal);
uint8_t contador=0;
uint16_t adc1, adc2;
uint16_t milivoltios;
char texto1[8];
char texto2[10];
char texto3[6];
int main(void)
{
	cli();

	DDRC &= ~((1 << PC2) | (1 << PC3)); // ADC2 y ADC3

	LCD_Init();
	UART_Init();

	LCD_SetCursor(0,0);
	LCD_String("S1:   S2:    S3:");
	sei();
	while (1)
	{
		//POT1 -> Voltaje
		adc1 = leerADC(2);
		milivoltios = (adc1 * 5000UL) / 1023;

		sprintf(texto1, "%u.%02uV", milivoltios / 1000, (milivoltios % 1000) / 10);

		LCD_SetCursor(1,6);
		LCD_String("     ");
		LCD_SetCursor(1,6);
		LCD_String(texto1);

		//POT2 -> Decimal
		adc2 = leerADC(3);
		sprintf(texto2, "%4u", adc2);

		LCD_SetCursor(1,0);
		LCD_String("    ");
		LCD_SetCursor(1,0);
		LCD_String(texto2);

		char c = UART_GetChar();
		if (c == '+') {
			contador++;
		}
		else if (c == '-') {
			contador--;
		}

		//S3
		sprintf(texto3, "%3d", contador);
		LCD_SetCursor(1,12);
		LCD_String("   ");     // limpia
		LCD_SetCursor(1,12);
		LCD_String(texto3);
		
		_delay_ms(300);
	}
}

uint16_t leerADC(uint8_t canal)
{
	ADMUX = (1 << REFS0) | (canal & 0x07);
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return ADC;
}

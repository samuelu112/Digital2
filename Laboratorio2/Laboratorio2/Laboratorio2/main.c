/*
 * Laboratorio2.c
 *
 * Created: 22/01/2026 19:14:28
 * Author : samur
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD/lcd.h"

uint8_t leerADC(uint8_t canal);

int main(void)
{
	uint8_t adc_valor;
	uint16_t milivoltios;
	char texto[8];

	DDRC &= ~(1 << PC2);   // PC2 como entrada (ADC2)

	LCD_Init();

	LCD_SetCursor(0,0);
	LCD_String("Voltaje");

	while (1)
	{
		adc_valor = leerADC(2);

		milivoltios = (adc_valor * 5000UL) / 255;

		sprintf(texto, "%u.%02uV",
		milivoltios / 1000,
		(milivoltios % 1000) / 10);

		LCD_SetCursor(1,0);
		LCD_String("     ");
		LCD_SetCursor(1,0);
		LCD_String(texto);

		_delay_ms(200);
	}
}

uint8_t leerADC(uint8_t canal)
{
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (canal & 0x07);
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	return ADCH;
}

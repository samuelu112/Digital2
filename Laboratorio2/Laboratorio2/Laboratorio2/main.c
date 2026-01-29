/*
 * Laboratorio2.c
 *
 * Created: 22/01/2026 19:14:28
 * Author : samur
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD/lcd.h"
#include "UART/UART.h"
#include "ADC/ADC.h"

uint16_t leerADC(uint8_t canal);
uint8_t contador = 0;
uint16_t adc1, adc2;
uint16_t milivoltios;
char texto1[8];
char texto2[10];
char texto3[6];
char texto[16];

int main(void)
{
	cli();
	
	DDRC = DDRC & ~((1 << PC2) | (1 << PC3)); //PC2 y PC3 como entradas
	
	LCD_Init();
	UART_Init();
	adc_iniciar();
	LCD_SetCursor(0,0);
	LCD_String("S1:   S2:    S3:");
	sei();
	
	while (1)
	{
		//Pot1 voltaje 2 decimales
		adc1 = adc_leer(2);  //PC2
		milivoltios = (adc1 * 5000UL) / 1023;
		
		
		//Muestra valor de pot1 en lcd (Voltaje
		sprintf(texto1, "%u.%02uV", milivoltios / 1000, (milivoltios % 1000) / 10);
		LCD_SetCursor(1,6);
		LCD_String(texto1);
		
		//Muestra valor de pot2 (Decimal)
		adc2 = adc_leer(3);  //PC3
		sprintf(texto2, "%4u", adc2);
		LCD_SetCursor(1,0);
		LCD_String(texto2);
		
		//Leer UART
		char c = UART_GetChar();
		if (c == '+') {
			contador++;
		}
		else if (c == '-') {
			contador--;
		}
		
		sprintf(texto3, "%3d", contador);
		LCD_SetCursor(1,12);
		LCD_String(texto3);
		
		sprintf(texto, "S1=%u.%02uV   S2=%4u", milivoltios / 1000, (milivoltios % 1000) / 10, adc2);
		UART_SendString(texto);
		UART_SendString("\r\n");
		
		_delay_ms(100);
	}
}

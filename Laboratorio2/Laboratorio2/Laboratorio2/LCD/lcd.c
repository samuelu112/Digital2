/*
 * lcd.c
 *
 * Created: 22/01/2026 19:15:25
 *  Author: samur
 */ 
#include "lcd.h"

// Enviar comando
void LCD_CMD(char cmd)
{
	PORTC &= ~RS;        // RS = 0 (comando)
	PORTD = cmd;        // Mandar comando al puerto D

	PORTC |= E;         // E = 1
	_delay_ms(4);
	PORTC &= ~E;        // E = 0
}

// Enviar caracter
void LCD_Char(char data)
{
	PORTC |= RS;        // RS = 1 (dato)
	PORTD = data;

	PORTC |= E;
	_delay_ms(4);
	PORTC &= ~E;
}

// Enviar texto
void LCD_String(char *str)
{
	while(*str)
	{
		LCD_Char(*str++);
	}
}

// Limpiar pantalla
void LCD_Clear(void)
{
	LCD_CMD(0x01);
	_delay_ms(2);
}

// Posicionar cursor
void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t pos;

	if(row == 0)
	pos = 0x80 + col;
	else
	pos = 0xC0 + col;

	LCD_CMD(pos);
}

// Inicialización LCD 8 bits
void LCD_Init(void)
{
	DDRD = 0xFF;          // Puerto D como salida (D0–D7)
	DDRC |= RS | E;      // PC0 y PC1 como salida

	PORTC = 0x00;

	_delay_ms(20);

	LCD_CMD(0x38);  // 8 bits, 2 líneas, 5x8
	LCD_CMD(0x0C);  // Display ON, cursor OFF
	LCD_CMD(0x06);  // Auto incremento
	LCD_CMD(0x01);  // Limpiar pantalla
}
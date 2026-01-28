/*
 * lcd.c
 *
 * Created: 22/01/2026 19:15:25
 *  Author: samur
 */ 
#include "lcd.h"

static void LCD_PulseEnable(void)
{
	PORTC |= E;
	_delay_us(1);
	PORTC &= ~E;
	_delay_ms(2);
}

static void LCD_WriteByte(uint8_t data)
{
	// D0–D3 -> PB0–PB3
	PORTB = (PORTB & 0xF0) | (data & 0x0F);

	// D4–D7 -> PD4–PD7
	PORTD = (PORTD & 0x0F) | (data & 0xF0);

	LCD_PulseEnable();
}

void LCD_CMD(uint8_t cmd)
{
	PORTC &= ~RS;
	LCD_WriteByte(cmd);
}

void LCD_Char(char data)
{
	PORTC |= RS;
	LCD_WriteByte(data);
}

void LCD_String(const char *str)
{
	while (*str)
	LCD_Char(*str++);
}

void LCD_Clear(void)
{
	LCD_CMD(0x01);
	_delay_ms(2);
}

void LCD_SetCursor(uint8_t fila, uint8_t col)
{
	uint8_t pos = (fila == 0) ? (0x80 + col) : (0xC0 + col);
	LCD_CMD(pos);
}

void LCD_Init(void)
{
	DDRB |= 0x0F;        // PB0–PB3 salida
	DDRD |= 0xF0;        // PD4–PD7 salida
	DDRC |= RS | E;     // RS y E

	PORTC &= ~(RS | E);

	_delay_ms(20);
	LCD_CMD(0x38);      // 8 bits, 2 líneas
	LCD_CMD(0x0C);      // Display ON
	LCD_CMD(0x06);      // Auto incremento
	LCD_CMD(0x01);      // Clear
}
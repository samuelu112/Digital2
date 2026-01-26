/*
 * lcd.h
 *
 * Created: 22/01/2026 19:15:07
 *  Author: samur
 */ 

#ifndef LCD_H_
#define LCD_H_


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Pines de control (Puerto C)
#define RS (1<<PC0)
#define E  (1<<PC1)

// Prototipos
void LCD_Init(void);
void LCD_CMD(char cmd);
void LCD_Char(char data);
void LCD_String(char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

#endif /* LCD_H_ */
/*
 * UART.h
 *
 * Created: 26/01/2026 09:54:39
 *  Author: samur
 */ 


#ifndef UART_H_
#define UART_H_

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(const char *str);

#endif /* UART_H_ */
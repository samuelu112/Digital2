/*
 * Lab1.c
 *
 * Created: 15/01/2026 19:09:38
 * Author : samur
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Display/Display.h"

uint8_t contador = 5;
uint8_t botones;
volatile uint8_t timer_activo = 0;

ISR(PCINT1_vect) {
	botones = PINC & 0x03;
	TCNT0 = 255;
	TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler 1024
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estado = PINC & 0x03;
	
	if (estado == botones && estado & 0x01 && !timer_activo) {
		// Iniciar Timer1
		TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC, prescaler 256
		OCR1A = 62499;
		TCNT1 = 0;
		TIMSK1 |= (1 << OCIE1A);
		timer_activo = 1;
	}
	PCICR |= (1 << PCIE1);
}

ISR(TIMER1_COMPA_vect) {
	if (contador > 0) {
		contador--;
		PORTD = seg7_convert(contador);
	}
	
	if (contador == 0) {
		TCCR1B = 0;
		TIMSK1 &= ~(1 << OCIE1A);
		timer_activo = 0;
	}
}

int main(void) {
	// Display
	DDRD = 0xFF;
	PORTD = seg7_convert(contador);
	
	// Botones
	DDRC &= ~0x03;  // PC0 y PC1 como entradas
	PORTC |= 0x03;  // Pull-ups
	
	// Configurar interrupciones
	PCICR |= (1 << PCIE1);
	PCMSK1 |= 0x03;
	TIMSK0 |= (1 << TOIE0);
	TIMSK1 &= ~(1 << OCIE1A);
	
	sei();
	
	while (1) {
		// Bucle principal
	}
}
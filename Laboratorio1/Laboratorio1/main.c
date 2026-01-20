/*
 * Lab1.c
 *
 * Created: 15/01/2026 19:09:38
 * Author : samur
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t contador = 5;
uint8_t botones;
uint16_t tima = 0;
uint8_t contar = 0;

uint8_t seg7_table[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
	0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

uint8_t display_digit;

ISR(PCINT1_vect) {
	botones = PINC & ((1 << PC0) | (1 << PC1));
	TCNT0 = 255;
	TCCR0B = (1 << CS02) | (1 << CS00);
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estadoa = PINC & ((1 << PC0) | (1 << PC1));
	
	if (estadoa == botones) {
		if (estadoa & (1 << PC0)) {
			TCCR1B = (1 << CS12);
			TCNT1 = 49911;
		}
	}
	PCICR |= (1 << PCIE1);
}

ISR(TIMER1_COMPA_vect) {
	if (contador > 0) {
		contador--;
		display_digit = seg7_table[contador];
		PORTD = display_digit;
	}
	
	if (contador == 0) {
		TCCR1B = 0;
	}
}

int main(void) {
	DDRD = 0xFF;
	DDRC &= ~0x03;
	PORTC |= 0x03;
	
	//Configuración del Timer1 para modo CTC 
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	OCR1A = 62499;
	
	TIMSK1 = (1 << OCIE1A);
	
	// Configuración para antirebote
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PC0) | (1 << PC1);
	TIMSK0 |= (1 << TOIE0);
	
	sei();
	
	display_digit = seg7_table[contador];
	PORTD = display_digit;
	
	while (1) {
	}
}


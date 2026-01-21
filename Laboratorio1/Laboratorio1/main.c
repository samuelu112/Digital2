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
uint8_t meta = 4;
uint8_t timer_activo = 0;
//Variables de juego
uint8_t j1 = 0;
uint8_t j2 = 0;
uint8_t juego = 1;
uint8_t ganador = 0;

void marcador1(){
	//Apagar LEDS
	PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
	PORTD &= ~(1 << PD7);
	
	if (j1 >= 4) {
		PORTD |= (1 << PD7) | seg7_table(1);
		PORTC &= (1 << PC3) | (1 << PC4) | (1 << PC5);
		} else if (j1 == 3) {
		PORTC |= ~((1 << PC3) | (1 << PC4)) | (1 << PC5);
		} else if (j1 == 2) {
		PORTC |= ~(1 << PC3) | (1 << PC4);
		} else if (j1 == 1) {
		PORTC |= (1 << PC3);
	}
}
void marcador2(){
	//Apagar LEDS
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
	
	if (j2 >= 4) {
		PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
		PORTD = seg7_table(2);
		} else if (j2 == 3) {
		PORTB |= ~((1 << PB0) | (1 << PB1)) | (1 << PB2);
		} else if (j2 == 2) {
		PORTB |= ~(1 << PB0) | (1 << PB1);
		} else if (j2 == 1) {
		PORTB |= (1 << PB0);
	}
}


ISR(PCINT1_vect) {
	botones = PINC & 0x07;
	TCNT0 = 255;
	TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler 1024
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estado = PINC & 0x07;
	
	if (estado == 0x01) {
		// Iniciar Timer1
		TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC, prescaler 256
		OCR1A = 62499;
		TCNT1 = 0;
		TIMSK1 |= (1 << OCIE1A);
	}else if (estado == 0x02){
		j1++;
		marcador1();
		if (j1 >= 4 ){
			j1 = 0;
		}
	}else if (estado == 0x04){
		j2++;
		marcador2();
		if (j2 >= 4 ){
			j2 = 0;
		}
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
		contador = 0;
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
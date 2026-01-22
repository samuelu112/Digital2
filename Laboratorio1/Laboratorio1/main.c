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

//variables de juego
uint8_t contador = 5;
uint8_t botones;
uint8_t timer_activo = 0;

uint8_t jugador1 = 0;
uint8_t jugador2 = 0;
uint8_t estado_juego = 0;// 0=esperando inicio, 1=conteo regresivo, 2=juego activo, 3=juego terminado
uint8_t ganador = 0;
uint8_t meta = 40;

//Estado anterior de botones para antirrebote
uint8_t estado_anterior_botones = 0x07;


//Función jugador 1
void marcador1(void) {
	//Apagar LEDS del jugador 1
	PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
	PORTD &= ~(1 << PD7);
	
	if (jugador1 >= 40) {
		PORTD |= (1 << PD7);
	}
	else if (jugador1 >= 30) {
		PORTC |= (1 << PC5);       
	}
	else if (jugador1 >= 20) {
		PORTC |= (1 << PC4);  
	}
	else if (jugador1 >= 10) {
		PORTC |= (1 << PC3);  
	}
}

//Función jugador 2
void marcador2(void) {
	//Apagar LEDS jugador 2
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
	
	if (jugador2 >= 40) {
		PORTB |= (1 << PB3);
	}
	else if (jugador2 >= 30) {
		PORTB |= (1 << PB2);
	}
	else if (jugador2 >= 20) {
		PORTB |= (1 << PB1);
	}
	else if (jugador2 >= 10) {
		PORTB |= (1 << PB0); 
	}
}

ISR(PCINT1_vect) {
	botones = PINC & 0x07;
	if ((estado_anterior_botones & 0x01) && !(botones & 0x01)) {//PC0 presionado
		if (estado_juego == 0) {
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);//Prescaler 1024
			PCICR &= ~(1 << PCIE1);
			timer_activo = 1;
		}
	}
	else if ((estado_anterior_botones & 0x02) && !(botones & 0x02)) {//PC1
		if (estado_juego == 2) {
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);
			PCICR &= ~(1 << PCIE1);
			timer_activo = 2;
		}
	}
	else if ((estado_anterior_botones & 0x04) && !(botones & 0x04)) {//PC2
		if (estado_juego == 2) {
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);
			PCICR &= ~(1 << PCIE1);
			timer_activo = 3;
		}
	}
	
	estado_anterior_botones = botones;
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estado_actual = PINC & 0x07;
	
	if (timer_activo == 1) {
		if (!(estado_actual & 0x01)) {
			//Conteo regresivo
			estado_juego = 1;
			contador = 5;
			PORTD = seg7_convert(contador);
			
			//timer1 para 1s
			TCCR1B = 0;
			TCNT1 = 0;
			OCR1A = 62499;
			TCCR1B = (1 << WGM12) | (1 << CS12);//prescaler 256
			TIMSK1 |= (1 << OCIE1A);
		}
	}
	else if (timer_activo == 2) {
		if (!(estado_actual & 0x02)) {
			if (jugador1 < meta && ganador == 0) {
				jugador1++;
				marcador1();
				
				if (jugador1 >= meta) {
					estado_juego = 3;
					ganador = 1;
					
					PORTC |= (1 << PC3) | (1 << PC4) | (1 << PC5);
					PORTD |= (1 << PD7);
					
					//Mostrar 1 en display
					PORTD = (PORTD & 0x80) | (seg7_convert(1) & 0x7F);
				}
			}
		}
	}
	else if (timer_activo == 3) {
		if (!(estado_actual & 0x04)) {
			if (jugador2 < meta && ganador == 0) {
				jugador2++;
				marcador2();
				
				if (jugador2 >= meta) {
					estado_juego = 3;
					ganador = 2;
					
					//Encender leds de jugador 2
					PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
					
					//Mostrar 2 en display
					PORTD = seg7_convert(2);
				}
			}
		}
	}
	
	timer_activo = 0;
	PCICR |= (1 << PCIE1);
}

ISR(TIMER1_COMPA_vect) {
	if (contador > 0) {
		contador--;
		PORTD = seg7_convert(contador);
		
		if (contador == 0) {
			TCCR1B = 0;
			TIMSK1 &= ~(1 << OCIE1A);
			estado_juego = 2;
			
			jugador1 = 0;
			jugador2 = 0;
			ganador = 0;
			//Apagar el display y PD7
			PORTD &= 0x00;
			
			//Apagar todos los LEDs
			PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
			PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
		}
	}
}

int main(void) {
	//PD0-PD6 para display de 7 segmentos Y PD7 para led jugador 1
	DDRD = 0xFF;  // Todos como salida
	PORTD = seg7_convert(contador);  // Mostrar 5 al inicio
	
	//PC0, PC1, PC2 para botones y PC3-PC5 para leds de jugador 1
	DDRC = (1 << PC3) | (1 << PC4) | (1 << PC5); 
	PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2);  //Poner en pullup
	
	//PB0-PB3 para LEDS jugador 2
	DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	PORTB = 0x00;
	
	//timer0
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	TCNT0 = 0;
	TIMSK0 = (1 << TOIE0);//Overflow
	
	//Timer1
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TIMSK1 = 0x00;
	
	//Interrupcion de botones
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);  //PC0, PC1, PC2
	
	//Variable de estado de botones
	estado_anterior_botones = PINC & 0x07;
	
	sei();
	
	while (1) {
	}
	
	return 0;
}
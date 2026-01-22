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

//Contador inicial
uint8_t contador = 5;
uint8_t botones;
uint8_t timer_activo = 0;

//Variables de juego
uint8_t j1 = 0;
uint8_t j2 = 0;
uint8_t estado_juego = 0;  //0=esperando inicio, 1=conteo regresivo, 2=juego activo, 3=juego terminado
uint8_t ganador = 0;
uint8_t meta = 4;

//Estado anterior de botones para antirebote
uint8_t boton_estado_anterior = 0x07;

//Función jugador 1
void marcador1(void) {
	//Apagar LEDS
	PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
	PORTD &= ~(1 << PD7);
	
	//Contador decadas jugador 1
	if (j1 >= 40) {
		PORTC |= ~(1 << PC5);
		PORTD |= (1 << PD7);
		} else if (j1 == 30) {
		PORTC |= ~(1 << PC4) | (1 << PC5);
		} else if (j1 == 20) {
		PORTC |= ~(1 << PC3) | (1 << PC4);
		} else if (j1 == 10) {
		PORTC |= (1 << PC3);
	}
}

//Función jugador 2
void marcador2(void) {
	//Apagar LEDS
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
	
	//Contador decadas jugador 2
	if (j2 >= 40) {
		PORTB |= ~(1 << PB2) | (1 << PB3);
		} else if (j2 == 30) {
		PORTB |= ~(1 << PB1) | (1 << PB2);
		} else if (j2 == 20) {
		PORTB |= ~(1 << PB0) | (1 << PB1);
		} else if (j2 == 10) {
		PORTB |= (1 << PB0);
	}
}

//Pinchange para botones del puerto c
ISR(PCINT1_vect) {
	botones = PINC & 0x07;
	//Detectar flanco descendente
	if ((boton_estado_anterior & 0x01) && !(botones & 0x01)) {//PC0 presionado
		if (estado_juego == 0) {
			//Iniciar timer0 para antirrebote
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);//Prescaler 1024
			PCICR &= ~(1 << PCIE1);//Deshabilitar interrupciones temporalmente
			timer_activo = 1;
		}
	}
	else if ((boton_estado_anterior & 0x02) && !(botones & 0x02)) {//PC1 presionado
		if (estado_juego == 2) {//Solo si el juego está activo
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);
			PCICR &= ~(1 << PCIE1);
			timer_activo = 2;
		}
	}
	else if ((boton_estado_anterior & 0x04) && !(botones & 0x04)) {//PC2 presionado
		if (estado_juego == 2) {//Solo si el juego está activo
			TCNT0 = 0;
			TCCR0B = (1 << CS02) | (1 << CS00);
			PCICR &= ~(1 << PCIE1);
			timer_activo = 3;
		}
	}
	
	boton_estado_anterior = botones;
}

//Antirrebote usando timer0
ISR(TIMER0_OVF_vect) {
	TCCR0B = 0; //detener timer0
	uint8_t estado_actual = PINC & 0x07;
	
	//Verificar después del antirrebote
	if (timer_activo == 1) {  //PC0 fue presionado
		if (!(estado_actual & 0x01)) {  //PC0 sigue presionado
			//Iniciar conteo regresivo
			estado_juego = 1;  //Conteo
			contador = 5;	//Inicio del conteo de 5 a 0
			PORTD = seg7_convert(contador);
			
			//Configurar timer1 para 1 segundo
			TCCR1B = 0;
			TCNT1 = 0;
			OCR1A = 62499;
			TCCR1B = (1 << WGM12) | (1 << CS12);  //modo CTC
			TIMSK1 |= (1 << OCIE1A);  //Habilitar interrupción
		}
	}
	else if (timer_activo == 2) {  //PC1 fue presionado
		if (!(estado_actual & 0x02)) {  //PC1 sigue presionado
			if (j1 < meta && ganador == 0) {
				j1++;
				marcador1();
				
				//Verificar quien gana
				if (j1 >= meta) {
					estado_juego = 3;  //Juego terminado
					ganador = 1;
					
					//Encender LEDs del jugador 1
					PORTC |= (1 << PC3) | (1 << PC4) | (1 << PC5);
					PORTD |= (1 << PD7);
					
					//Mostrar 1 en display
					PORTD = seg7_convert(1);
				}
			}
		}
	}
	else if (timer_activo == 3) {  //PC2 fue presionado
		if (!(estado_actual & 0x04)) {  //PC2 sigue presionado
			if (j2 < meta && ganador == 0) {
				j2++;
				marcador2();
				
				//Verificar si ganó
				if (j2 >= meta) {
					estado_juego = 3;  //Juego terminado
					ganador = 2;
					
					//Encender LEDs del jugador 2
					PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
					
					//Mostrar 2 en display
					PORTD = seg7_convert(2);
				}
			}
		}
	}
	
	timer_activo = 0;
	PCICR |= (1 << PCIE1);  //Habilitar interrupciones
}

//Conteo regresivo
ISR(TIMER1_COMPA_vect) {
	if (contador > 0) {
		contador--;
		PORTD = seg7_convert(contador);
		
		if (contador == 0) {
			//Detener timer1
			TCCR1B = 0;
			TIMSK1 &= ~(1 << OCIE1A);
			
			//Cambiar a estado de juego
			estado_juego = 2;
			
			// Inicializar marcadores
			j1 = 0;
			j2 = 0;
			ganador = 0;
			
			// Apagar display
			PORTD = 0x00;
			
			// Apagar todos los LEDs
			PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
			PORTD &= ~(1 << PD7);
			PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
		}
	}
}

int main(void) {
	// Configuración de puertos
	
	//PD0-PD6 para display de 7 segmentos, PD7 para LED jugador 1
	DDRD = 0xFF;  //Todos como salida
	PORTD = seg7_convert(contador);  //Mostrar 5 inicialmente
	
	//PC0, PC1, PC2 para botones, PC3-PC5 para LEDS jugador 1
	DDRC = (1 << PC3) | (1 << PC4) | (1 << PC5);  //PC3-PC5 como salidas
	PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2);  //Habilitar pull-up en PC0, PC1, PC2
	
	//PB0-PB3 para LEDS jugador 2
	DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	PORTB = 0x00;  //apagado al principio
	
	//Configurar timer0 para antirrebote
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	TCNT0 = 0;
	TIMSK0 = (1 << TOIE0);  //interrupción por overflow
	
	//timer1 para conteo regresivo
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TIMSK1 = 0x00;
	
	//Configurar interrupciones de botones
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);  //PC0, PC1, PC2
	
	//Variable de botones
	boton_estado_anterior = PINC & 0x07;
	
	sei();
	
	while (1) {
		
	}
	
	return 0;
}
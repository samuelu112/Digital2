/*
 * ADC.c
 *
 * Created: 29/01/2026 08:45:01
 *  Author: samur
 */ 
#include "ADC.h"

//Inicializa ADC
void adc_iniciar(void) {
	//Referencia AVcc, ajuste a la derecha
	ADMUX = (1 << REFS0);
	
	//Habilitar ADC con divisor 128
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	//Lectura inicial
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
}

//Lee un canal ADC
uint16_t adc_leer(uint8_t canal) {
	//Seleccionar canal (0-7)
	ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
	
	//Iniciar conversión
	ADCSRA |= (1 << ADSC);
	
	//Esperar a que termine
	while (ADCSRA & (1 << ADSC));
	
	//Retornar valor
	return ADC;
}
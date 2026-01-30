/*
 * ADC.h
 *
 * Created: 29/01/2026 08:44:20
 *  Author: samur
 */ 


#ifndef ADC_H_
#define ADC_H_


#include <avr/io.h>
#include <stdint.h>

void adc_iniciar(void);
uint16_t adc_leer(uint8_t canal);


#endif /* ADC_H_ */
/*
 * ATtiny45.cpp
 *
 * Created: 7/29/2017 6:15:54 PM
 *  Author: orencollaco
 */ 


#include "ATtiny45.h"

volatile uint64_t time;

void SPI_MasterInit(){
	
}

uint8_t SPI_MasterTransmit(uint8_t data){
	register uint8_t a,b;
	a = (1 << USIWM0) | (1 << USITC);
	b = (1 << USIWM0) | (1 << USITC) | (1 << USICLK);
	//USIDR = a;
	//USIDR = b;
	USIDR = data;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	USICR = a;
	USICR = b;
	return USIDR;
}

void init_CTC_T0(uint8_t Count, uint8_t prescale){
	TCCR0B &= 0xF8;
	prescale &= 0x07;
	TCCR0B |= prescale;
	TCCR0A = (1 << WGM01);
	OCR0A = Count;
}

void init_PWM_T1(uint8_t prescale, bool chA, bool chB){
	OCR1C = 0xFF;
	prescale &= 0x07;
	TCCR1 = 0;
	GTCCR = 0;
	if(chA){
		TCCR1 |= (1 << PWM1A) | (1 << COM1A1);
		DDRB |= (1 << 1);
	}
	if(chB){
		GTCCR |= (1 << PWM1B) | (1 << COM1B1);	
		DDRB |= (1 << 4);
	}
	TCCR1 |= prescale;
	GTCCR |= 0x60;
	PLLCSR = 0x06;
	TIMSK = (1 << 2);
}

void ADC_Init(unsigned char admux, unsigned char adcsra)
{
	ADCSRA = adcsra;
	ADMUX = admux;
}

void ADC_ChangeChannelTo(unsigned char admux)
{
	admux&=0x0F;
	admux|=admux_c;
	ADMUX = admux;
}

unsigned char ADC_Read(unsigned char admux)
{
	admux&=0x0F;
	admux|=admux_c;
	ADMUX = admux;
	while(!(ADCSRA & (1<<ADIF)))
	;
	PORTB ^= (1<<5);
	return ADCH;
}

void ADC_Start()
{
	ADCSRA = adcsra_c | (1<<ADSC);
}

uint64_t millis(){
	return time;
}
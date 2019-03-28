/*
 * ATtiny45.h
 *
 * Created: 7/29/2017 6:16:22 PM
 *  Author: orencollaco
 */ 


#ifndef ATTINY45_H_
#define ATTINY45_H_

#define F_CPU 1000000UL
//#define UART_SUPPORTED
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>

#define adcsra_c 0xA2
#define admux_c 0x20

typedef struct bitstruct
{
	unsigned char bit0 : 1;
	unsigned char bit1 : 1;
	unsigned char bit2 : 1;
	unsigned char bit3 : 1;
	unsigned char bit4 : 1;
	unsigned char bit5 : 1;
	unsigned char bit6 : 1;
	unsigned char bit7 : 1;
}bit_field;
#define GET_BITFIELD(addr)( *((volatile bit_field*)&(addr))  )

void ADC_Init( unsigned char, unsigned char);

void ADC_ChangeChannelTo(unsigned char admux);

unsigned char ADC_Read(unsigned char );

void ADC_Start();



void init_CTC_T0(uint8_t Count, uint8_t prescale);

void init_PWM_T1(uint8_t prescale, bool chA, bool chB);

uint8_t sendSPICommand(uint8_t cmd);

uint8_t SPI_MasterTransmit(uint8_t data);

uint64_t millis();

void SPI_MasterInit();

#endif /* ATTINY45_H_ */
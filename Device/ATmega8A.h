/*
 * ATmega8A.h
 *
 * Created: 7/23/2017 5:18:15 PM
 *  Author: orencollaco
 */ 

#define F_CPU 16000000UL
#ifndef ATMEGA8A_H_
#define ATMEGA8A_H_
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//I2C
#define MTA_SLAW_ACK 0x18
#define MTD_SLAW_ACK 0x28
#define MTDS_SLAR_ACK 0x40
#define MTDR_SLAR_NACK 0x58
#define START 0x08
#define RESTART 0x10

//ADC Parameters
#define ADMUX_VCC_NOADLAR							0x40
#define ADMUX_VCC_ADLAR								0x60
#define ADC_RES										1024
#define VREF										3.300
#define ADCSRA_FCPUBY32_NOINTERRUPT					0xA5
#define ADCSRA_FCPUBY32_NOINTERRUPT_SINGLE			0x85

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

//Device I/O register definitions for compatibility
#define MCUSR MCUCR
#define WDIE 7

void SPI_MasterInit(void);
uint8_t SPI_MasterTransmit(char cData);
void USART_Init( unsigned int ubrr);
unsigned int USART_Receive( void );
void USART_Transmit9( unsigned int data );
void USART_Transmit(uint8_t data);
void USART1_Transmit(uint8_t data);
void print(const char *p);

void ADC_Init(unsigned char admux, unsigned char adcsra);
void ADC_ChangeChannelTo(unsigned char admux);
uint16_t ADC_Read();
uint8_t ADC_ReadChannel(uint8_t admux);
void ADC_Start();
uint8_t ADC_Finish();

void setLEDIndicatorCount(uint16_t count1);
void stopLEDIndicator();
void startLEDIndicator();
void setLEDIndicatorMode(uint8_t mode);

void init_T1_FASTPWM();
void init_T1_CTC();
void init_T2_CTC();
void init_PWM_T1(uint8_t prescale, bool chA, bool chB);

bool isI2CGood();
void I2C_Init(unsigned char twbr, unsigned char twsr);
unsigned char I2C_Read(unsigned char daddr, unsigned char raddr);
void I2C_Write(unsigned char daddr, unsigned char raddr, unsigned char data);
void I2C_WriteCommand(uint8_t daddr, uint8_t cmd);
void I2C_Error(unsigned char twsr);

void WDT_off(void);

uint64_t millis();


#endif /* ATMEGA8A_H_ */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
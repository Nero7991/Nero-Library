/*
 * ATmega328P.h
 *
 * Created: 8/18/2017 9:14:15 PM
 *  Author: orencollaco
 */ 

//#define F_CPU 8000000
#ifndef ATMEGA328P_H_
#define ATMEGA328P_H_
#include "ProjectDef.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>

//
#define PORT_B 0x00
#define PORT_C 0x01
#define PORT_D 0x02

#define DDR_B 0x00
#define DDR_C 0x01
#define DDR_D 0x02

#define INPUT 0x00
#define OUTPUT 0x01

#define LOW 0x00
#define HIGH 0x01

//ADC Parameters
#define ADMUX_VCC_NOADLAR							0x40
#define ADMUX_VCC_ADLAR								0x60
#define ADMUX_1V1_NOADLAR							0xE0
#define ADC_RES										1024.0L
#define ADCSRA_FCPUBY32_NOINTERRUPT					0xA5
#define ADCSRA_FCPUBY32_NOINTERRUPT_SINGLE			0x85
#define ADCSRA_FCPUBY16_INTERRUPT_FREERUN			0xAC

//I2C
#define MTA_SLAW_ACK 0x18
#define MTD_SLAW_ACK 0x28
#define MTDS_SLAR_ACK 0x40
#define MTDR_SLAR_NACK 0x58
#define START 0x08
#define RESTART 0x10

#define WDTCR WDTCSR

void getReadyToSleep();

void wakeUp();

void allowSleep(bool state);

bool sleepAllowed();

void initSPISlave();

void enableSPI(bool set);

void enableSPIInterrupt(bool set);

void SPI_MasterInit(void);

uint8_t SPI_MasterTransmit(char cData);

void USART_Init( unsigned int ubrr);

unsigned char USART_Receive();

void USART_Transmit(uint8_t data);

void USART_Enable(bool set);

void print(const char *p);

void enableADC(bool enable);

void ADC_Init(unsigned char admux, unsigned char adcsra);

void ADC_ChangeChannelTo(unsigned char admux);

uint16_t ADC_Read();

uint8_t ADC_ReadChannel(uint8_t admux);

void ADC_Start();

void ADC_Finish();

void Init_PWM_T0(unsigned char prescale);

void stop_OC0A_OUTPUT();

void start_OC0A_OUTPUT();

void stop_OC0B_OUTPUT();

void start_OC0B_OUTPUT();

void Init_PWM_T1(unsigned char prescale);

void Init_CTC_T1(uint8_t prescale, uint16_t count);

void Init_PWM_T2(unsigned char prescale);

void Init_CTC_T2(uint8_t prescale, uint8_t count);

void Init_CTC_T2_ExtOsc(uint8_t prescale, uint8_t count);

void init_INT0(uint8_t mode);

void init_INT0(uint8_t mode);

void setPinState(uint8_t port, uint8_t pin, bool set);

void setPinDirection(uint8_t port, uint8_t pin, bool direction);

bool getPinState(uint8_t port, uint8_t pin);

void setPinChangeInterrupt(uint8_t port, uint8_t pin, bool set);

bool isI2CGood();

bool isI2CPowered();

void enableI2CPower(bool set);

void enableI2C(bool set);

void initI2CComplete(bool set);

bool isI2CInitialized();

void powerCycleI2C();

void I2C_Init(unsigned char twbr, unsigned char twsr);

unsigned char I2C_Read(unsigned char daddr, unsigned char raddr);

void I2C_Write(unsigned char daddr, unsigned char raddr, unsigned char data);

void I2C_WriteCommand(uint8_t daddr, uint8_t cmd);

void I2C_Error(unsigned char twsr);

void powerDownFor(uint8_t prescale);
void resetIn(uint8_t prescale);
void WDT_Prescaler_Change(void);
void WDT_Init();
void WDT_off(void);

void setCountDown(uint16_t cDown);
uint16_t getCountDown();
uint64_t millis();
void milliHappened();

void setLEDIndicatorCount(uint16_t count1);
void stopLEDIndicator();
void startLEDIndicator();
void setLEDIndicatorMode(uint8_t mode);

void init_INT0(uint8_t mode);
void init_INT1(uint8_t mode);

#endif /* ATMEGA328P_H_ */
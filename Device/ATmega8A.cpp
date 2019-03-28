/*
 * ATmega8A.cpp
 *
 * Created: 7/23/2017 5:17:53 PM
 *  Author: orencollaco
 */ 

#include "ATmega8A.h"

const unsigned char FadeLookUp[] PROGMEM =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
	0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C,
	0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F,
	0x51, 0x54, 0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76, 0x79, 0x7C,
	0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A, 0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA,
	0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
	0xD0, 0xD2, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
	0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2,
	0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8,
	0xF9, 0xF9, 0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC,
	0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF};

uint64_t time;

uint16_t lcount;

uint8_t cdown, CountD = 2;

bool I2C_OK = 1;


void SPI_MasterInit(void)
{
	/* Sets MOSI and SCK as output all others inputs */
	DDRB = (1 << 3) | (1 << 5) | (1 << 2) | (1 << 0);
	/* Set SS bar high */
	PORTB = (1 << 2) | (1 << 0);
	/* Enable SPI as Master and set clock to FCPU/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	SPSR = (1 << 0);
}

uint8_t SPI_MasterTransmit(char cData){
	/* Start Transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

void init_FastPWM_T1(){
	TCCR1A = 0x81;
	TCCR1B = 0x09;
	TIMSK |= (1 << TOIE1);
	DDRB |= (1 << 1);
	//OCR1A = 1000;
	sei();
}

void init_CTC_T1(){
	
}

void init_CTC_T2(){
	TCCR2 = (1 << WGM21) | (1 << CS22);  //FCPU/64
	OCR2 = 249;
	TIMSK |= (1 << OCIE2);
	sei();
}

void init_T0_CTC(uint8_t prescale){
	TCCR0 = 0x03;
}

void init_PWM_T1(uint8_t prescale, bool chA, bool chB){
	prescale &= 0x07;
	TCCR1A = 0;
	TCCR1B = 0;
	if(chA){
		TCCR1A |= (1 << COM1A1);
		DDRB |= (1 << 1);
	}
	if(chB){
		TCCR1A |= (1 << COM1B1);
		DDRB |= (1 << 2);
	}
	TCCR1A |= (1 << WGM10);
	TCCR1B |= prescale;
	
}

void USART_Init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSRB =(1<<RXEN) | (1<<UCSZ2) | (1<<TXEN);
	/* Set Frame Format : 9 data, 2 stop bits */
	UCSRC = (1<<URSEL)|(1<<UCSZ0) | (1<<UCSZ1);
}

void USART_Transmit9( unsigned int data )
{
	/* Wait for empty transmit Buffer */
	while(!(UCSRA & (1<<UDRE)));
	UCSRB &= ~(1<<TXB8);
	if (data & 0x100)
	UCSRB |= (1<<TXB8);
	UDR = data;
}

void USART_Transmit(uint8_t data){
	/* Wait for empty transmit Buffer */
	while(!(UCSRA & (1 << UDRE)));
	/* Put data in the buffer */
	UDR = data;
}

void USART1_Transmit(uint8_t data){
	
}

unsigned int USART_Receive( void )
{
	unsigned char status, resh, resl;
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get status and 9th bit, then data */
	/* from buffer */
	status = UCSRA;
	resh = UCSRB;
	resl = UDR;
	/* If error, return -1 */
	if (status & ((1<<FE)|(1<<DOR)|(1<<2)))
	return -1;
	/* Filter the 9th bit, then return */
	resh = (resh >> 1) & 0x01;
	return ((resh << 8) | resl);
}

void print(const char *p){
	USART_Transmit('\r');
	USART_Transmit('\n');
	while(*p != 0){
		USART_Transmit(*p);
		p++;
	}
}

void ADC_Init(unsigned char admux, unsigned char adcsra)
{
	ADMUX = admux;
	ADCSRA = adcsra;
}

void ADC_ChangeChannelTo(unsigned char admux)
{
	admux &= 0x0F;
	ADMUX &= 0xF0;
	ADMUX |= admux;
}

uint16_t ADC_Read()
{
	while(!(ADCSRA & (1<<ADIF)))
	;
	return ADC;
}

uint8_t ADC_ReadChannel(uint8_t admux){
	admux &= 0x0F;
	ADMUX &= 0xF0;
	ADMUX |= admux;
	while(!(ADCSRA & (1<<ADIF)));
	return ADCH;
}

void ADC_Start()
{
	ADCSRA |= (1<<ADSC);
}

uint8_t ADC_Finish(){
	while(!(ADCSRA & (1<<ADIF)));
	return ADC;
}

void I2C_Init(unsigned char twbr, unsigned char twsr){
	/* Set bit rate register */
	TWBR = twbr;
	/* Mask status bits */
	twsr &= 0x03;
	TWSR |= twsr;
	/* Enable TWI Interface */
}

void I2C_Error(unsigned char twsr){
	#ifdef DEBUG_UART
	USART_Transmit(twsr);
	print("I2C Error");
	#endif
	I2C_OK = 1;
	//TWCR = 0;
//	I2C_Init(1,1);
	//I2C_Write(0x68,0x6B,0);
}

bool isI2CGood(){
	return I2C_OK;
}

unsigned char I2C_Read(unsigned char daddr, unsigned char raddr){
	if(I2C_OK){
		uint8_t data;
		/* Transmit Start condition */
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != START)
		I2C_Error(TWSR & 0xF8);
		TWDR = daddr << 1;
		
		/* Transmit device address & R/W(w) bit */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTA_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		TWDR = raddr;
		
		/* Transmit device register address */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTD_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		
		/* Transmit Start condition */
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != RESTART)
		I2C_Error(TWSR & 0xF8);
		/* Read Mode(SLA+R) SLA = Slave Address, R = 1*/
		TWDR = daddr << 1 | 0x01;
		
		/* Transmit device address */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTDS_SLAR_ACK)
		I2C_Error(TWSR & 0xF8);
		
		/* Receive device data */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTDR_SLAR_NACK)
		I2C_Error(TWSR & 0xF8);
		data = TWDR;
		
		/* Transmit STOP */
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
		cdown = CountD;
		while((TWCR & (1<<TWSTO)) && cdown);
		return data;
	}
	else
	return 0;
}

void I2C_Write(unsigned char daddr, unsigned char raddr, unsigned char data){
	if(I2C_OK){
		/* Transmit Start condition */
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != START)
		I2C_Error(TWSR & 0xF8);
		/* Write Mode(SLA+W)  SLA = Slave Address, W = 0*/
		TWDR = daddr << 1;
		
		/* Transmit device address & R/W(w) bit */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTA_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		TWDR = raddr;
		
		/* Transmit device register address */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTD_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		TWDR = data;
		
		/* Transmit data */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTD_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		
		/* Transmit STOP */
		TWCR = (1 << TWSTO) | (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while((TWCR & (1<<TWSTO)) && cdown);
	}
}

void I2C_WriteCommand(uint8_t daddr, uint8_t cmd){
	if(I2C_OK){
		/* Transmit Start condition */
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != START)
		I2C_Error(TWSR & 0xF8);
		/* Write Mode(SLA+W)  SLA = Slave Address, W = 0*/
		TWDR = daddr << 1;
		
		/* Transmit device address & R/W(w) bit */
		TWCR = (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while(!(TWCR & (1<<TWINT)) && cdown);
		if ((TWSR & 0xF8) != MTA_SLAW_ACK)
		I2C_Error(TWSR & 0xF8);
		TWDR = cmd;
		
		/* Transmit STOP */
		TWCR = (1 << TWSTO) | (1<<TWINT) | (1<<TWEN);
		cdown = CountD;
		while((TWCR & (1<<TWSTO)) && cdown);
	}
}

void WDT_off(void)
{
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCR = 0x00;
	sei();
}

uint64_t millis(){
	return time;
}

void setLEDIndicatorCount(uint16_t count1){
	lcount = count1;
}

void stopLEDIndicator(){
	
}

void startLEDIndicator(){
	
}


ISR(TIMER1_COMPA_vect){
	//PORTD ^= (1 << 6);
}

ISR(TIMER2_COMP_vect){
	time += 1;
	if(cdown)
	cdown -= 1;
}
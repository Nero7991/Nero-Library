/*
 * ATmega328P.cpp
 *
 * Created: 8/18/2017 9:13:51 PM
 *  Author: orencollaco
 */ 

#include "ATmega328P.h"

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

volatile uint64_t time;

volatile uint16_t lcount;

volatile uint16_t cdown, CountD = 2, I2C_FailCount;

volatile bool I2C_OK = 1, I2CPower = 0, I2C_InitComplete = 0;

bool AllowSleep;

void getReadyToSleep(){
	PRR = (1 << PRTWI) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);
}

void wakeUp(){
	PRR = 0;
}

void allowSleep(bool state){
	AllowSleep = state;
}

bool sleepAllowed(){
	return AllowSleep;
}



void SPI_MasterInit(void)
{
	/* Sets MOSI and SCK as output all others inputs */
	DDRB &= ~(1 << 4);
	DDRB |= (1 << 3) | (1 << 5) | (1 << 2);
	/* Set SS bar high */
	PORTB |= (1 << 2);
	/* Enable SPI as Master and set clock to FCPU/16 */
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	SPSR |= (1 << SPI2X);
}

void initSPISlave(){
	/* Sets MOSI and SCK as output all others inputs */
	DDRB &= ~((1 << 3) | (1 << 5) | (1 << 2));
	DDRB |= (1 << 4);
	/* Enable SPI as Slave*/
	SPCR &= ~(1 << MSTR);
	SPCR |= (1 << SPE);
}

void enableSPIInterrupt(bool set){
	if(set)
	SPCR |= (1 << SPIE);
	else
	SPCR &= ~(1 << SPIE);
}

uint8_t SPI_MasterTransmit(char cData){
	/* Start Transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

void Init_PWM_T0(unsigned char prescale){
	TCCR0A = 0xA1;
	prescale &= 0x07;
	TCCR0B |= prescale;
	DDRD |= (1 << 5) | (1 << 6);
}

void stop_OC0A_OUTPUT(){
	TCCR0A &= 0x3F;
}

void start_OC0A_OUTPUT(){
	TCCR0A |= 0x80;
}

void stop_OC0B_OUTPUT(){
	TCCR0A &= 0xCF;
}

void start_OC0B_OUTPUT(){
	TCCR0A |= 0x20;
}

void Init_PWM_T1(unsigned char prescale){
	TCCR1A = 0xA1;
	prescale &= 0x07;
	TCCR1B = prescale;
	DDRB |= 0x1E;
}

void Init_CTC_T1(uint8_t prescale, uint16_t count){
	OCR1A = count;
	TCCR1A = 0;
	prescale &= 0x07;
	prescale |= (1 << WGM12);
	TCCR1B = prescale;
	TIMSK1 = (1 << OCIE1A);
	sei();
}

void Init_PWM_T2(unsigned char prescale){
	TCCR2A = 0xA1;
	prescale &= 0x07;
	TCCR2B = prescale;
	DDRD |= (1 << 3);
	DDRB |= (1 << 3);
	TIMSK2 = (1 << 0);
}

void Init_CTC_T2(uint8_t prescale, uint8_t count){
	TCCR2A = 0x52;
	prescale &= 0x07;
	TCCR2B |= prescale;
	TIMSK2 = (1 << OCIE2A);
}

void Init_CTC_T2_ExtOsc(uint8_t prescale, uint8_t count){
	// Clear interrupts
	TIMSK2 = 0;
	TIFR2 = 0;
	// Set clock source
	ASSR = (1 << AS2);

	// Init counter
	TCNT2 = 0;

	OCR2A = count;
	OCR2B = 0;

	TCCR2A = 0;
	TCCR2B = 0;

	// check ASSR to ensure writes worked
	// wait for TCN2xUB OCR2xUB and TCR2xUB
	print("Waiting...ASSR");
	while(ASSR & 0x1F);
	print("Done.");

	// clear interrupt flags
	_delay_ms(10);
	TIFR2 = 0;

	// Start counter
	prescale &= 0x07;
	TCCR2A = (1 << WGM21);
	TCCR2B = prescale;
	ASSR = (1 << AS2);
	print("Waiting...ASSR");
	while(ASSR & 0x1F);
	print("Done.");
	// wait for clock to stabilize
	_delay_ms(1000);

	// Start interrupts
	TIMSK2 = (1 << OCIE2A);
	//_delay_ms(1000);
}

void init_INT0(uint8_t mode){
	mode &= (1 << 0) | (1 << 1);
	EICRA &= ~((1 << 0) | (1 << 1));
	EICRA |= mode;
	EIMSK |= (1 << 0);
	
}

void init_INT1(uint8_t mode){
	mode &= (1 << 0) | (1 << 1);
	EICRA &= ~((1 << 2) | (1 << 3));
	EICRA |= (mode << 2);
	EIMSK |= (1 << 1);
}

void setPinState(uint8_t port, uint8_t pin, bool set){
	switch(port){
		case 0:
		if(set)
		PORTB |= (1 << pin);
		else
		PORTB &= ~(1 << pin);
		break;
		case 1:
		if(set)
		PORTC |= (1 << pin);
		else
		PORTC &= ~(1 << pin);
		break;
		case 2:
		if(set)
		PORTD |= (1 << pin);
		else
		PORTD &= ~(1 << pin);
		break;
	}
}

void setPinDirection(uint8_t port, uint8_t pin, bool direction){
	switch(port){
		case 0:
		if(direction)
		DDRB |= (1 << pin);
		else
		DDRB &= ~(1 << pin);
		break;
		case 1:
		if(direction)
		DDRC |= (1 << pin);
		else
		DDRC &= ~(1 << pin);
		break;
		case 2:
		if(direction)
		DDRD |= (1 << pin);
		else
		DDRD &= ~(1 << pin);
		break;
	}
}

void setPinChangeInterrupt(uint8_t port, uint8_t pin, bool set){
	PCICR |= (1 << port);
	switch(port){
		case 0:
		PCMSK0 |= (set << pin);
		break;
		case 1:
		PCMSK1 |= (set << pin);
		break;
		case 2:
		PCMSK2 |= (set << pin);
		break;
	}
}

bool getPinState(uint8_t port, uint8_t pin){
	switch(port){
		case 0:
		return (PINB & (1 << pin));
		break;
		case 1:
		return (PINC & (1 << pin));
		break;
		case 2:
		return (PIND & (1 << pin));
		break;
	}
	return 0;
}

void USART_Init(unsigned int ubrr) {
/*Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8); 
UBRR0L = (unsigned char)ubrr;
/*Enable receiver and transmitter */
UCSR0B = (1 << RXEN0) | (1 << TXEN0);// | (1 << RXCIE0);
/* Set frame format : 8 data bits, 2 stop bits */
UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void USART_Transmit(unsigned char data){
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1 << UDRE0)));
	/* Put the data into the buffer, data 
	is sent serially once written */
	UDR0 = data;
}

unsigned char USART_Receive() {
/* Wait for data to be received */
while (!(UCSR0A & (1<<RXC0)));
/* Get and return received data from buffer */ 
return UDR0;
}

void USART_Enable(bool set){
	if(set)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	else
	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
}

void print(const char *p){
	//USART_Transmit('\r');
	//USART_Transmit('\n');
	while(*p != 0){
		USART_Transmit(*p);
		p++;
	}
}

void enableADC(bool enable){
	if (enable)
	{
		ADCSRA |= (1 << ADEN);
	} 
	else
	{
		ADCSRA &= ~(1 << ADEN);
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
	ADCSRA |= (1 << ADIF);
	return ADC;
}

uint8_t ADC_ReadChannel(uint8_t admux){
	admux &= 0x0F;
	ADMUX &= 0xF0;
	ADMUX |= admux;
	while(!(ADCSRA & (1<<ADIF)))
	;
	ADCSRA |= (1 << ADIF);
	return ADCH;
}

void ADC_Start()
{
	ADCSRA |= (1 << ADSC);
}

void ADC_Finish(){
	while(!(ADCSRA & (1<<ADIF)));
}

//#ifndef NO_MILLIS
uint64_t millis(){
	return time;
}
//#endif

void I2C_Init(unsigned char twbr, unsigned char twsr){
	/* Set bit rate register */
	TWBR = twbr;
	/* Mask status bits */
	twsr &= 0x03;
	TWSR |= twsr;
	/* Enable TWI Interface */
}

void enableI2C(bool set){
	if(set)
	TWCR &= ~(1 << TWEN);
	else
	TWCR |= (1 << TWEN);
}

void I2C_Error(unsigned char twsr){
	if(I2C_FailCount > 20){
		powerCycleI2C();
		I2C_FailCount = 0;
		return;	
	}
	if(UCSR0B & (1 << TXEN0)){
		USART_Transmit('i');
		//USART_Transmit(twsr);
		//print("I2C Error ");
	}
	I2C_FailCount += 1;
	I2C_OK = 0;
}

bool isI2CGood(){
	return I2C_OK;
}

#ifdef I2C_POWER_CONTROLLABLE
void enableI2CPower(bool set){
	if(set)
	I2C_POWER_EN = 1;
	else
	I2C_POWER_EN = 0;
	I2CPower = set;
}
#endif

bool isI2CPowered(){
	return I2CPower;
}

void powerCycleI2C(){
	I2C_InitComplete = 0;	
}

void initI2CComplete(bool set){
	I2C_InitComplete = set;
}

bool isI2CInitialized(){
	return I2C_InitComplete;
}

unsigned char I2C_Read(unsigned char daddr, unsigned char raddr){
	uint8_t data;
	/* Transmit Start condition */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	cdown = CountD;
	while(!(TWCR & (1<<TWINT)) && cdown);
	if(!cdown){
		I2C_Error(0xAA);
		I2C_OK = 0;
		return 0;
	}
	else{
		I2C_OK = 1;
	}
	if ((TWSR & 0xF8) != START){
		I2C_Error(TWSR & 0xF8);
		I2C_OK = 0;
		return 0;
	}
	else{
		I2C_OK = 1;
	}
	
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

void I2C_Write(unsigned char daddr, unsigned char raddr, unsigned char data){
	/* Transmit Start condition */
	cdown = CountD;
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while(!(TWCR & (1<<TWINT)) && cdown)
	if(!cdown){
		I2C_OK = 0;
		I2C_Error('c');
		return;
	}
	else{
		I2C_OK = 1;
	}
	if((TWSR & 0xF8) != START){
		I2C_Error(TWSR & 0xF8);
		I2C_OK = 0;
		return;
	}
	else{
		I2C_OK = 1;
	}
	/* Write Mode(SLA+W)  SLA = Slave Address, W = 0*/
	TWDR = daddr << 1;
	/* Transmit device address & R/W(w) bit */
	cdown = CountD;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while((!(TWCR & (1<<TWINT))) && cdown);
	if ((TWSR & 0xF8) != MTA_SLAW_ACK)
	I2C_Error(TWSR & 0xF8);
	TWDR = raddr;
	
	/* Transmit device register address */
	cdown = CountD;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while((!(TWCR & (1<<TWINT))) && cdown);
	if ((TWSR & 0xF8) != MTD_SLAW_ACK)
	I2C_Error(TWSR & 0xF8);
	TWDR = data;
	
	/* Transmit data */
	cdown = CountD;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while((!(TWCR & (1<<TWINT))) && cdown);
	if ((TWSR & 0xF8) != MTD_SLAW_ACK)
	I2C_Error(TWSR & 0xF8);
	
	/* Transmit STOP */
	cdown = CountD;
	TWCR = (1 << TWSTO) | (1<<TWINT) | (1<<TWEN);
	while((TWCR & (1<<TWSTO)) && cdown);
}

void I2C_WriteCommand(uint8_t daddr, uint8_t cmd){
	/* Transmit Start condition */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	cdown = CountD;
	while(!(TWCR & (1<<TWINT)) && cdown);
	if(!cdown){
		I2C_Error(0xAA);
		I2C_OK = 0;
		return;
	}
	else{
		I2C_OK = 1;
	}
	if ((TWSR & 0xF8) != START){
		I2C_Error(TWSR & 0xF8);
		I2C_OK = 0;
		return;
	}
	else{
		I2C_OK = 1;
	}
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

void WDT_Init(){
	asm volatile("cli"::);
	asm volatile("wdr"::);
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCR |= (1<<WDCE) | (1<<WDE);
	WDTCR |= (1 << WDIE);
	/* Turn off WDT */
	WDTCR = 0x00;
	asm volatile("sei"::);
}

void WDT_Prescaler_Change(void)
{
	asm volatile("cli"::);
	asm volatile("wdr"::);
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Start timed sequence */
	WDTCR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler (time-out) value = 64K cycles (~0.5 s) */
	WDTCR = (1<<WDP2) | (1<<WDP0) | (1<<WDIE);
	asm volatile("sei"::);
	//asm volatile("sei"::);
	//MCUSR |= (1 << SE) | (1 << SM1);
	//asm volatile("sleep"::);
}

void setLEDIndicatorCount(uint16_t count1){
	lcount = count1;
}

void stopLEDIndicator(){
	OCR2B = 0;
	TCCR2B &= 0xF8;
	TCCR2A &= 0x0F;
}

void startLEDIndicator(){
	TCCR2B |= 0x01;
	TCCR2A |= 0xA0;
}

void setCountDown(uint16_t cDown){
	//cdown = cDown;
}

uint16_t getCountDown(){
	return cdown;
}

void powerDownFor(uint8_t prescale){
	uint8_t t;
	t = 0x07 & prescale;
	prescale &= 0x08;
	prescale = prescale << 2;
	prescale |= t;
	prescale |= (1 << WDIE) | (1 << WDE);
	WDTCR = prescale;
	asm volatile("cli"::);
	asm volatile("wdr"::);
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Start timed sequence */
	WDTCR = (1<<WDCE) | (1<<WDE);
	/* Set new prescaler (time-out) value = 64K cycles (~0.5 s) */
	WDTCR = prescale;
	asm volatile("sei"::);
	//sleep_bod_disable();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sleep_enable();
	sleep_mode();
}

void resetIn(uint8_t prescale){
	uint8_t t;
	t = 0x07 & prescale;
	prescale &= 0x08;
	prescale = prescale << 2;
	prescale |= t;
	prescale |= (1 << WDE);
	WDTCR = prescale;
	asm volatile("cli"::);
	asm volatile("wdr"::);
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Start timed sequence */
	WDTCR = (1<<WDCE) | (1<<WDE);
	/* Set new prescaler (time-out) value = 64K cycles (~0.5 s) */
	WDTCR = prescale;
	asm volatile("sei"::);
}


void milliHappened(){
	time += 1;
	if(cdown != 0){
		cdown -= 1;
	}
	//USART_Transmit('m');
	//FLASH = 1;
}

/*This ISR will be called every millisecond
and will increment time variable to keep track of milliseconds
passed*/

#ifndef NO_MILLIS
ISR(TIMER1_COMPA_vect){			//Handles the millis function
	
	time += 1;
	if(cdown)
		cdown -= 1;
	
}
#endif

/*
ISR(TIMER2_OVF_vect){
	static uint8_t i,up = 1;
	static uint16_t count1; //Max Count = 224
	if(count1 >= lcount){
		count1 = 0;
		if( i < 223 && up){
			i += 1;
			OCR2B = pgm_read_byte(&(FadeLookUp[i]));
		}
		if(i == 223 && up){
			up = 0;
		}
		if(i > 0 && !up){
			i -= 1;
			OCR2B = pgm_read_byte(&(FadeLookUp[i]));
		}
		if(i == 0 && !up){
			up = 1;
		}
	}
	count1 += 1;
}*/


ISR(WDT_vect){
	MCUSR &= ~(1 << SE);
}
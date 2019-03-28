/*
 * Power.cpp
 *
 * Created: 8/19/2017 8:29:53 PM
 *  Author: orencollaco
 */ 
#include "Power.h"

void powerDownFor1(uint8_t prescale){
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
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_mode();
}


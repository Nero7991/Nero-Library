/*
 * AVR.h
 *
 * Created: 8/30/2017 3:53:29 PM
 *  Author: orencollaco
 */ 


#ifndef AVR_H_
#define AVR_H_




#if defined (__AVR_ATmega328P__)
#include "ATmega328P.h"
#elif defined (__AVR_ATmega8A__)
#include "ATmega8A.h"
#elif defined (__AVR_ATmega8__)
#include "ATmega8A.h"
#elif defined (__AVR_ATtiny45__)
#include "ATtiny45.h"
#elif defined (__AVR_ATtiny25__)
#include "ATtiny45.h"
#endif



//#include "ProjectDef.h"
#endif 
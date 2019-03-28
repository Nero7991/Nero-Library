/*
 * PIDControl.h
 *
 * Created: 8/22/2017 4:38:29 PM
 *  Author: orencollaco
 */ 


#ifndef PIDCONTROL_H_
#define PIDCONTROL_H_

#include "Essential.h"

class PIDControl{
	
private:
	int16_t PError, DError, IError, Target, Error, MaxError, Output;
	float PGain, DGain, IGain;
	uint16_t Result;
	uint8_t count;
	uint64_t t;
	
public:

PIDControl(int16_t target, float Pgain, float Dgain, float Igain);

PIDControl();

PIDControl(int16_t target);

void setMaxError(int16_t maxError);

int16_t stayAtTarget(int16_t current);

void setTarget(int16_t target);

void setGain(float Pgain, float Dgain, float Igain);

void incrementGainP();

void decrementGainP();

void incrementGainD();

void decrementGainD();

void incrementGainI();

void decrementGainI();

};



#endif /* PIDCONTROL_H_ */
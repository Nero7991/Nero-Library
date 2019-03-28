/*
 * PIDControl.cpp
 *
 * Created: 8/22/2017 4:38:10 PM
 *  Author: orencollaco
 */ 

#include "PIDControl.h"

PIDControl::PIDControl(int16_t target, float Pgain, float Dgain, float Igain){
	Target = target;
	PGain = Pgain;
	DGain = Dgain;
	IGain = Igain;
	Output = 0;
}

PIDControl::PIDControl(){
	Output = 0;
}

PIDControl::PIDControl(int16_t target){
	Target = target;
	Output = 0;
}

void PIDControl::setMaxError(int16_t maxError){
	MaxError = maxError;
}

void PIDControl::setTarget(int16_t target){
	Target = target;
	Output = 0;
}

void PIDControl::setGain(float Pgain, float Dgain, float Igain){
	PGain = Pgain;
	DGain = Dgain;
	IGain = Igain;
}

void PIDControl::incrementGainP(){
	PGain += 0.1;
}

void PIDControl::decrementGainP(){
	PGain -= 0.1;
}

void PIDControl::incrementGainD(){
	DGain += 0.1;
}

void PIDControl::decrementGainD(){
	DGain -= 0.1;
}

void PIDControl::incrementGainI(){
	IGain += 0.1;
}

void PIDControl::decrementGainI(){
	IGain -= 0.1;
}

int16_t PIDControl::stayAtTarget(int16_t current){
	if(millis() - t > 10){
		t = millis();
	}
	else{
		return Output;
	}
	Error = Target - current;
	if(Error > MaxError || Error < -MaxError){
		return 0;
	}
	count += 1;
	if(count == 1){
	PError = Error;
	DError = Error;
	IError = Error;
	return Output;
	}
	if(count == 2){
		DError = DError - Error;
		IError = IError + Error;
		Output = PGain * PError + DGain * DError ;//+ IGain * IError;
		count = 0;
	}
	Output = Output * -1;
	Output /= 100.0;
	if(Output > 80){ 
		Output = 80;
	}
	if(Output < -80){
		Output = -80;
	}
		#ifdef DEBUG_UART
		printString("P=");
		printNumber(PGain * 10);
		printString("D=");
		printNumber(DGain * 10);
		printString("I=");
		printNumber(IGain * 10);
		printSignedNumber(current);
		printSignedNumber(PError);
		printSignedNumber(DError);
		printSignedNumber(IError);
		printString("Output = ");
		printSignedNumber(Result);
		#endif
	return Output;
}
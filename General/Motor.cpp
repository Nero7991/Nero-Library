/*
 * Motor.cpp
 *
 * Created: 8/20/2017 4:35:38 PM
 *  Author: orencollaco
 */ 
#include "Motor.h"

Motor::Motor(){
	M1D1_D = M1D2_D = M2D1_D = M2D2_D = 1;
	M1D1 = M1D2 = M2D1 = M2D2 = 0;
	Speed = 0;
	Speed1 = &OCR0A;
	Speed2 = &OCR0B;
	*Speed1 = 0;
	*Speed2 = 0;
}

void Motor::setSpeedL(uint8_t PWM){
	*Speed1 = PWM;
}

void Motor::setSpeedDirectionL(int16_t PWM){
	if(PWM == 0){
		disableBreaksL();
	}
	if(PWM > 0){
		Speed = PWM;
		setForwardDirectionL(Speed);
	}
	if(PWM < 0){
		Speed = -1 * PWM;
		setBackwardDirectionL(Speed);
	}
}


void Motor::setBackwardDirectionL(uint8_t PWM){
	#ifdef DEBUG_UART
	printString("BL ");
	printNumber(PWM);
	#endif
	if(Direction1 != BACKWARD){
		stop_OC0A_OUTPUT();
		PWML = 0;
		_delay_us(2);
		M1D1 = 0;
		M1D2 = 1;
		_delay_us(2);
		start_OC0A_OUTPUT();
		*Speed1 = PWM;
		Direction1 = BACKWARD;
	}
	else{
		*Speed1 = PWM;
	}
}

void Motor::setForwardDirectionL(uint8_t PWM){
	#ifdef DEBUG_UART
	printString("FL ");
	printNumber(PWM);
	#endif
	if(Direction1 != FORWARD){
		stop_OC0A_OUTPUT();
		PWML = 0;
		_delay_us(2);
		M1D1 = 1;
		M1D2 = 0;
		_delay_us(2);
		start_OC0A_OUTPUT();
		*Speed1 = PWM;
		Direction1 = FORWARD;
	}
	else{
		*Speed1 = PWM;
	}
}

void Motor::enableBreaksL(){
	setSpeedL(0);
	M1D1 = 0;
	M1D1 = 0;
	Direction1 = STOP;
}

void Motor::disableBreaksL(){
	setSpeedL(0);
	M1D1 = 1;
	M1D2 = 1;
	Direction1 = FREE_RUN;
}

void Motor::setSpeedR(uint8_t PWM){
	*Speed2 = PWM;
}

int16_t Motor::getSpeedR(){
	return SpeedR;
}

void Motor::setSpeedDirectionR(int16_t PWM){
	SpeedR = PWM;
	if(PWM == 0){
		disableBreaksR();
	}
	if(PWM > 0){
		Speed = PWM;
		setForwardDirectionR(Speed);
	}
	if(PWM < 0){
		Speed = -1 * PWM;
		setBackwardDirectionR(Speed);
	}	
}

void Motor::setBackwardDirectionR(uint8_t PWM){
	#ifdef DEBUG_UART
	printString("BR ");
	printNumber(PWM);
	#endif
	
	if(Direction2 != BACKWARD){
		stop_OC0B_OUTPUT();
		PWMR = 0;
		_delay_us(2);
		M2D1 = 0;
		M2D2 = 1;
		_delay_us(2);
		start_OC0B_OUTPUT();
		*Speed2 = PWM;
		Direction2 = BACKWARD;
	}
	else{
		*Speed2 = PWM;
	}
}
void Motor::setForwardDirectionR(uint8_t PWM){
	#ifdef DEBUG_UART
	printString("FR ");
	printNumber(PWM);
	#endif
	if(Direction2 != FORWARD){
		stop_OC0B_OUTPUT();
		PWMR = 0;
		_delay_us(2);
		M2D1 = 1;
		M2D2 = 0;
		_delay_us(2);
		start_OC0B_OUTPUT();
		*Speed2 = PWM;
		Direction2 = FORWARD;
	}
	else{
		*Speed2 = PWM;
	}
}

void Motor::disableBreaksR(){
	SpeedR = 0;
	setSpeedR(0);
	M2D1 = 1;
	M2D2 = 1;
	Direction2 = FREE_RUN;
}

void Motor::enableBreaksR(){
	SpeedR = 0;
	setSpeedR(0);
	M2D1 = 0;
	M2D2 = 0;
	Direction2 = STOP;
}

int16_t Motor::offsetForJoy(uint16_t data)
{
	data >>= 1;
	if(data > 255)
	{
		data -= 255;
		if(data > JOY_UP_THRESHOLD)
			data = 255;
		if(data < JOY_DOWN_THRESHOLD)
			data = 0;
		t = -1 * data;
	}
	else
	{
		data = 255 - data;	
		if(data > JOY_UP_THRESHOLD)
		data = 255;
		if(data < JOY_DOWN_THRESHOLD)
		data = 0;
		t = data;
	}
	
	#ifdef DEBUG_UART
	printSignedNumber(t);
	#endif
	
	return t;
}

uint8_t Motor::joyToSwitch(uint16_t data){
	data >>= 1;
	if(data > 255)
	{
		data -= 255;
		if(data > JOY_UP_THRESHOLD - 30)
		return 1;
		if(data < JOY_DOWN_THRESHOLD + 30)
		return 0;
	}
	else
	{
		data = 255 - data;
		if(data > JOY_UP_THRESHOLD - 30)
		return 2;
		if(data < JOY_DOWN_THRESHOLD + 30)
		return 0;
	}
	return 0;
}

void Motor::updateMotorSpeed(uint16_t gas, uint16_t steer){
	Gas		=	offsetForJoy(gas);
	Steer	=	offsetForJoy(steer);
	if(Gas + Steer <= 255 && Gas + Steer >= -255){
		setSpeedDirectionR(Gas + Steer);
	}
	if(Gas - Steer <= 255 && Gas - Steer >= -255){
		setSpeedDirectionL(Gas - Steer);
	}
	if(Gas + Steer > 255){
		setSpeedDirectionR(255);
	}
	if(Gas + Steer < -255){
		setSpeedDirectionR(-255);
	}
	if(Gas - Steer > 255){
		setSpeedDirectionL(255);
	}
	if(Gas - Steer < -255){
		setSpeedDirectionL(-255);
	}
}

void Motor::updateMotorSpeed_RearDrive(uint16_t Gas, uint16_t Steer){
	setSpeedDirectionR(offsetForJoy(Gas));
	setSpeedDirectionL(offsetForJoy(Steer));
}

void Motor::shutdown(){
	#ifdef STATS
	Notify(PSTR("Shutting down Motor..."));
	#endif
	Init_PWM_T0(0);
	stop_OC0B_OUTPUT();
	stop_OC0A_OUTPUT();
	enableBreaksL();
	enableBreaksR();
	#ifdef STATS
	Notify(PSTR("Done."));
	#endif
}

void Motor::wakeUp(){
	#ifdef STATS
	Notify(PSTR("Waking up Motor..."));
	#endif
	Init_PWM_T0(1);
	start_OC0A_OUTPUT();
	start_OC0B_OUTPUT();
	disableBreaksL();
	disableBreaksR();
	#ifdef STATS
	Notify(PSTR("Motor awake."));
	#endif
}
/*
 * Motor.h
 *
 * Created: 8/20/2017 4:35:56 PM
 *  Author: orencollaco
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

#define FORWARD					01
#define BACKWARD				02
#define FREE_RUN				03
#define STOP					04

#define JOY_UP_THRESHOLD		250
#define JOY_DOWN_THRESHOLD		20

#include "Essential.h"

class Motor{
	
private:
	volatile uint8_t *Speed1, *Speed2;
	int16_t Gas, Steer, t, SpeedL, SpeedR;
	uint8_t Speed;
		
public:

	uint8_t Direction1, Direction2;
	
	Motor();
	
	void setForwardDirectionL(uint8_t PWM);
	
	void setBackwardDirectionL(uint8_t PWM);
	
	void setSpeedL(uint8_t PWM);
	
	void setSpeedDirectionL(int16_t PWM);
	
	void toggleDirectionL();
	
	void enableBreaksL();
	
	void disableBreaksL();
	
	void setForwardDirectionR(uint8_t PWM);
		
	void setBackwardDirectionR(uint8_t PWM);
		
	void setSpeedR(uint8_t PWM);
	
	int16_t getSpeedR();
	
	void setSpeedDirectionR(int16_t PWM);
		
	void toggleDirectionR();
		
	void enableBreaksR();
		
	void disableBreaksR();
	
	int16_t offsetForJoy(uint16_t data);
	
	uint8_t joyToSwitch(uint16_t data);
	
	void updateMotorSpeed(uint16_t Gas, uint16_t Steer);
	
	void updateMotorSpeed_RearDrive(uint16_t Gas, uint16_t Steer);
	
	void shutdown();
	
	void wakeUp();
};



#endif /* MOTOR_H_ */
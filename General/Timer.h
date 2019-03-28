/*
 * Timer.h
 *
 * Created: 11/3/2017 2:56:11 PM
 *  Author: orencollaco
 */ 


#ifndef TIMER_H_
#define TIMER_H_


#include "Essential.h"

#define TIMER_MODE_REPEAT 01
#define TIMER_MODE_ONCE   00

#define TIMER1_COUNT_1MS	F_CPU/1000

class TimerClass{
	
		public:
			
			static uint8_t i;
			
			static uint64_t Time;
			
			static TimerClass *Timers[30];
			
			static void begin();
			
			static void callAllTimerObjects();
			
			static uint64_t millis();
			
			void initializeTimer();
			
			uint16_t Count, Count_Save;
			
			uint8_t Mode, Timer_ID;
			
			uint16_t NCount, NCount_Save; 
					
			bool NCount_EN, Count_EN;
			
			Fptr CallBackPtr;	
			
			void setCallBackTime(uint16_t time, uint8_t mode, Fptr func);
			
			uint16_t getCallBackTime();
			
			void resetCallbackTimer();
			
			void setTime(uint16_t time);
			
			uint16_t getTime();
			
			void resetTimer();	
			
			void milliPassed();
			
			uint8_t getTimerID();
			
};

extern TimerClass Timer;

#endif /* TIMER_H_ */
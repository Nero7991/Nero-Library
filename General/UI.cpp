/*
 * UI.cpp
 *
 * Created: 8/30/2017 9:39:23 PM
 *  Author: orencollaco
 */ 



#include "UI.h"

UI::UI(OLED_01 *D1, NRF24L01 *n1){
	#ifdef STATS
	Notify(PSTR("Initializing UI..."));
	#endif
	N = n1;
	Display = D1;
	Str = D1;
	initialize();
}

void UI::initialize(){
	SW1_D = 0;
	SW2_D = 0;
	SW1_PO = 1;
	SW2_PO = 1;
	B1 = 0;
	Ch1 = 0;
	CarBatt = 0;
	CarMode = 0;
	FirstRun = 1;
	DebugMode = 0;
	ADC_ChangeChannelTo(2);
	for(uint8_t i = 0; i < 5; i += 1){
		ADC_Start();
		Batt_Voltage = ADC_Read();
	}
	ADC_ChangeChannelTo(0);
	for (uint8_t i = 0; i < 5; i += 1)
	{
		PayLoad[i] = 0;
	}
	Display->enablePower(false);
	Display->displayFrame(OLED_BootLogo);
	Display->enablePower(true);
	_delay_ms(1000);
	Display->clearDisplay();
	#ifdef STATS
	Notify(PSTR("UI Initialization complete."));
	#endif
}

uint8_t* UI::updateUI(){
	if((uint16_t)millis() - t > 5){
		t = millis();
		if(Ch1){
			ADC_Start();
			ADC_T = ADC_Read();
			PayLoad[2] = ADC_T;
			PayLoad[3] = ADC_T >> 8;
			Ch1 = 0;
			ADC_ChangeChannelTo(Ch1);
		}
		else{
			ADC_Start();
			ADC_T = ADC_Read();
			PayLoad[0] = ADC_T;
			PayLoad[1] = ADC_T >> 8;
			Ch1 = 1;
			ADC_ChangeChannelTo(Ch1);
		}
		updateBattVoltage();
		Speed = N->transmitPayload(PayLoad);
		if(!N->isRXEmpty()){
			N->readACKPayload(InPayLoad);
			CarBatt = InPayLoad[0] | (InPayLoad[1] << 8);
			CarMode = InPayLoad[2] & 0x03;
		}
		if(!Speed){
			CarBatt = 0;	
		}
		RefreshRatet += 1;
	}
	if(SW1){
		B1 &= ~(1 << 0);
	}
	else{
		B1 |= (1 << 0);
	}
	if(SW2){
		B1 &= ~(1 << 1);
	}
	else{
		B1 |= (1 << 1);
	}
	if(FirstRun && B1){
		DebugMode = 1;
	}
	
	PayLoad[4] = B1;
	
	if(DebugMode){
		N->printInfo();
	    printPayload();
	}
	
	#ifdef DEBUG_OLED
	static uint8_t t2;
	if((uint8_t)((uint8_t)millis() - t2) > 100){
		
		t2 = millis();
		
		Display->goToLine(0);
		Display->setColumnStart(0);
		Str->printString("A = ");
		Str->printNumber(PayLoad[2] | (PayLoad[3] << 8));
		Str->printNumber(PayLoad[0] | (PayLoad[1] << 8));
		Str->printString("D = ");
		Str->printNumber(PayLoad[4]);
		Display->clearLineAhead();
		
		Display->goToLine(2);
		Display->setColumnStart(0);
		Str->printString("B/s=");
		Str->printNumber(Speed);
		Str->printString("FPS=");
		Str->printNumber(RefreshRate);
		Str->printString("M=");
		Str->printNumber(CarMode);
		Display->clearLineAhead();
		
		Display->goToLine(4);
		Display->setColumnStart(0);
		Str->printString("Remote Batt=");
		Str->printNumberWithPoint(Batt_Voltage * (VREF/ADC_RES) * (R1 + R2)/R1 * CALI * 1000,3);
		Str->printString("V");
		Display->clearLineAhead();
		
		Display->goToLine(6);
		Display->setColumnStart(0);
		Str->printString("Car Batt=");
		Str->printNumberWithPoint(CarBatt,3);
		Str->printString("V");
		Display->clearLineAhead();	
	}
	#endif
	
	if((uint16_t)millis() - t1 > 1000){
		RefreshRate = RefreshRatet;
		RefreshRatet = 0;
		t1 = millis();
	}
	
	if(FirstRun){
		FirstRun = 0;
	}
	
	return PayLoad;
}

void UI::updateBattVoltage(){
	ADC_ChangeChannelTo(2);
	ADC_Start();
	Batt_Voltage += ADC_Read();
	Batt_Voltage /= 2;
	ADC_ChangeChannelTo(Ch1);
}
void UI::printPayload(){
	printStringCRNL("ADC = ");
	printNumber(PayLoad[2] | (PayLoad[3] << 8));
	printNumber(PayLoad[0] | (PayLoad[1] << 8));
	printString(" Data = ");
	printNumber(PayLoad[4]);
}
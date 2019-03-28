/*
 * Essential.h
 *
 * Created: 7/23/2017 5:09:13 PM
 *  Author: orencollaco
 */ 


#ifndef ESSENTIAL_H_
#define ESSENTIAL_H_

#include "AVR.h"

//DEBUG

bool getFlowControl();
void setFlowControl(bool flag);

void Notify(const char* str);
void printHexNumber(uint64_t data,uint8_t length);
void printStringNL(char *p);
void printStringCRNL(const char *p);
void printString(const char *p);
void printChar(unsigned char data);
void printNumber(uint64_t hex);
void printNumberA(uint64_t hex);
void printNumberWithPoint(uint64_t hex, uint8_t point);
void printSignedNumber(int16_t num);

#ifdef ATmega328PB
void printStringNL1(char *p);
void printStringCRNL1(const char *p);
void printString1(const char *p);
void printChar1(unsigned char data);
#endif

//House keeping functions
char* hexArrayToString(uint8_t *data, uint8_t length);
uint8_t* numToArray(uint64_t data);
unsigned char hexToASCII(unsigned char data);
unsigned char charToHex(char *p);
char* hexToString(unsigned char data);
void copyArray(uint8_t *a1, uint8_t *a2, uint8_t length);
uint32_t hexToDecimal(uint16_t hex);

#endif /* ESSENTIAL_H_ */
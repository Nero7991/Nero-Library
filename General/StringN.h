/*
 * StringN.h
 *
 * Created: 9/8/2017 2:42:32 PM
 *  Author: orencollaco
 */ 


#ifndef STRINGN_H_
#define STRINGN_H_

#include "AVR.h"

class StringN {
	
private:
	
	
	
public:
	
	void Notify(const char* str);
		
	void printHexNumber(uint64_t data,uint8_t length);
		
	void printStringNL(char *p);
		
	void printStringCRNL(const char *p);
		
	void printString(const char *p);
	
	void printStringRight(const char *p);
		
	virtual void printChar(uint8_t data);
		
	void printNumber(uint64_t hex);
		
	void printNumberWithPoint(uint64_t hex, uint8_t point);
		
	void printSignedNumber(int16_t num);
	
	char* hexArrayToString(uint8_t *data, uint8_t length);
	
	uint8_t* numToArray(uint64_t data);
	
	unsigned char hexToASCII(unsigned char data);
	
	unsigned char charToHex(char *p);
	
	char* hexToString(unsigned char data);
	
	void copyArray(uint8_t *a1, uint8_t *a2, uint8_t length);
	
	uint32_t hexToDecimal(uint16_t hex);
	
};




#endif /* STRINGN_H_ */
/*
 * StringN.cpp
 *
 * Created: 9/8/2017 2:42:16 PM
 *  Author: orencollaco
 */ 

#include "StringN.h"

char* StringN::hexToString(uint8_t data){
	static char ascii[5],temp;
	temp = data;
	data = data>>4;
	ascii[0] = hexToASCII(data);
	temp &= 0x0F;
	ascii[1] = hexToASCII(temp);
	ascii[2] = '\0';
	return ascii;
}

void StringN::printHexNumber(uint64_t data,uint8_t length){
	printString(hexArrayToString(numToArray((data)),length));
}

uint8_t* StringN::numToArray(uint64_t data){
	static uint8_t array[8];
	for(uint8_t i = 0; i < 8; i++){
		array[i] = data;
		data >>= 8;
	}
	return array;
}

char* StringN::hexArrayToString(uint8_t *data, uint8_t length){
	static char str[64], t;
	data += length - 1;
	printChar('0');
	printChar('x');
	for (uint8_t i = 0; i < length; i++)
	{
		t = *data;
		printString(hexToString(t));
		data -= 1;
	}
	str[length + 2] = 0;
	return str;
}

uint8_t StringN::charToHex(char *p){
	unsigned char t;
	if(*p >= 30 && *p <= 39)
	{
		*p -= 0x30;
		t = (*p << 4);
	}
	p++;
	if(*p >= 30 && *p <= 39)
	{
		*p -= 0x30;
		t = *p;
	}
	else
	t = 0;
	return t;
}

uint8_t StringN::hexToASCII(uint8_t data){
	if(data > 0x09){
		data -= 0x0A;
		data += 0x41;
		return data;
	}
	else{
		data += 0x30;
		return data;
	}
}

void StringN::printSignedNumber(int16_t num){
	if(num & (1 << 15)){
		num -= 1;
		num ^= 0xFFFF;
		printChar('-');
		printNumber((uint16_t)num);
	}
	else{
		printNumber((uint16_t)num);
	}
}

void StringN::printNumber(uint64_t hex){
	uint8_t str[100], count = 0;
	if(hex == 0){
		printChar('0');
	}
	while(hex > 0)
	{
		str[count] = ((hex % 10) + 0x30);
		hex /= 10;
		count += 1;
	}
	while(count != 0){
		count -= 1;
		printChar(str[count]);
	}
}

void StringN::printNumberWithPoint(uint64_t hex, uint8_t point){
	uint8_t str[100], count = 0;
	while(hex > 0)
	{
		str[count] = ((hex % 10) + 0x30);
		hex /= 10;
		count += 1;
	}
	while(count != 0){
		count -= 1;
		printChar(str[count]);
		if(count == point)
		printChar('.');
	}
	printChar(' ');
}

uint32_t StringN::hexToDecimal(uint16_t hex){
	uint32_t temp = 0,power = 1;
	while(hex > 0)
	{
		temp += (hex % 10) * power ;
		hex /= 10;
		power *= 0x10;
	}
	return temp;
}

void StringN::Notify(const char* str){
	printChar('\r');
	printChar('\n');
	while(pgm_read_byte(str) != 0){
		printChar(pgm_read_byte(str));
		str += 1;
	}
}


void StringN::printStringNL(char *p){
	printChar('\n');
	while(*p != 0){
		printChar(*p);
		p++;
	}
}

void StringN::printStringCRNL(const char *p){
	printChar('\r');
	printChar('\n');
	while(*p != 0){
		printChar(*p);
		p++;
	}
}

void StringN::printString(const char *p){
	while(*p != 0){
		printChar(*p);
		p++;
	}
}

void StringN::printChar(uint8_t data){
	USART_Transmit(data);
}
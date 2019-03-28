/*
 * OLED_01.cpp
 *
 * Created: 9/5/2017 7:26:42 PM
 *  Author: orencollaco
 */ 

#include "OLED_01.h"

OLED_01::OLED_01(){
	
}

OLED_01::OLED_01(uint8_t Addr){
	addr = Addr;
	_delay_ms(100);
	initialize();
}

OLED_01::OLED_01(uint8_t Addr, uint8_t addr_Mode){
	addr = Addr;
	Addr_Mode = addr_Mode;
	_delay_ms(100);
	initialize();
}

void OLED_01::initialize(){
	sendCommand(SSD1306_DISPLAYOFF);                    // 0xAE
	sendCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	sendCommand(0xF0);                                  // the suggested ratio 0x80

	sendCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
	sendCommand(SSD1306_LCDHEIGHT - 1);

	sendCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	sendCommand(0x0);                                   // no offset
	sendCommand(SSD1306_SETSTARTLINE | 0x0);            // line #0
	sendCommand(SSD1306_CHARGEPUMP);                    // 0x8D
	
	sendCommand(SSD1306_SETCOMPINS);                    // 0xDA
	sendCommand(0x12);
	sendCommand(SSD1306_SETCONTRAST);                   // 0x81
	sendCommand(0xCF);
	setSegmentRemap(true);
	setCOMRemap(true);
	setStartLine(0);
	setAddressingMode(Addr_Mode);
	initI2CComplete(1);
	FontSize = 1;
	Inverted = 0;
	InvertMode = 0;
}

void OLED_01::setDeviceAddress(uint8_t Addr){
	addr = Addr;
}

uint8_t OLED_01::readStatus(){
	cli();
	return I2C_Read(addr, (1 << 6));
	sei();
}

uint8_t OLED_01::scanI2CDevices(){
	return 0;
}

void OLED_01::sendCommand(uint8_t cmd){
	Cbyte = 0;
	I2C_Write(addr, Cbyte, cmd);
}

void OLED_01::ssd1306_command(uint8_t cmd){
	Cbyte = 0;
	I2C_Write(addr, Cbyte, cmd);
}

void OLED_01::setContrastTo(uint8_t contrast){
	sendCommand(SET_CONTRAST);
	sendCommand(contrast);
}

void OLED_01::enablePower(bool set){
	if(set){
		sendCommand(DISPLAY_ON);
	}
	else{
		sendCommand(DISPLAY_OFF);
	}
	PowerON = set;
}

bool OLED_01::getPowerEnabled(){
	return PowerON;
}

void OLED_01::setEntireDisplayOn(){
	sendCommand(ENTIRE_DISPLAY_ON);
}

void OLED_01::setDisplayToGDDRAM(){
	sendCommand(DISPLAY_GDDR);
}

void OLED_01::invertDisplay(bool set){
	if(set){
		Inverted = 1;
		sendCommand(DISPLAY_INVERTED);
	}
	else{
		Inverted = 0;
		sendCommand(DISPLAY_NOT_INVERTED);
	}
}

void OLED_01::setAddressingMode(uint8_t mode){
	sendCommand(SSD1306_MEMORYMODE);
	sendCommand(mode);
}

void OLED_01::setPageAddress(uint8_t start, uint8_t stop){
	sendCommand(SSD1306_PAGEADDR);
	sendCommand(start);
	sendCommand(stop);
}

void OLED_01::goToLine(uint8_t start){
	Rptr = start;
	start &= 0x07;
	start |= 0xB0;
	sendCommand(start);
}

void OLED_01::setColumnAddress(uint8_t start, uint8_t stop){
	sendCommand(SSD1306_COLUMNADDR);
	sendCommand(start);
	sendCommand(stop);
}

void OLED_01::setColumnStart(uint8_t start){
	Cptr = start;
	sendCommand(0x10 | start >> 4);
	sendCommand(start & 0x0F);
}

void OLED_01::setStartLine(uint8_t data){
	sendCommand(SSD1306_SETSTARTLINE);
	sendCommand(data);
}

void OLED_01::testOLED(){
	setAddressingMode(HORIZONTAL_ADDRESSING);
	setColumnAddress(0,127);
	setPageAddress(0,7);
	for(uint16_t i = 0; i < 1024; i++){
		writeToRAM(0xFF);
	}
	_delay_ms(1000);
	for(uint16_t i = 0; i < 1024; i++){
		writeToRAM(0);
	}
	_delay_ms(1000);
	setAddressingMode(Addr_Mode);
}

void OLED_01::writeToRAM(uint8_t data){
	Cbyte = (1 << 6);
	I2C_Write(addr, Cbyte, data);
	Cptr += 1;
	if(Cptr > 127){
		Cptr = 0;
		if(Rptr > 6){
			Rptr = 0;
			goToStart();
			return;
		}
		goToNextLine();
	}
}


void OLED_01::clearLineAhead(uint8_t column){
	if(Cptr < column){
		uint16_t ColSave;
		uint8_t RowSave;
		ColSave = Cptr;
		RowSave = Rptr;
		if(FontSize == 1){
			while(Cptr != column){
				writeToRAM(0);
			}
			goToLine(RowSave);
			setColumnStart(ColSave);
			return;
		}
		if(FontSize > 1){
			ColSave = Cptr;
			for (uint8_t i = 0; i < FontSize; i += 1)
			{
				goToLine(RowSave + i);
				setColumnStart(ColSave);
				while(Cptr != column){
					writeToRAM(0);
				}
			}
			goToLine(RowSave);
			setColumnStart(ColSave);
		}
	}
}

void OLED_01::clearLineAhead(){
	clearLineAhead(127);
}

void OLED_01::setSegmentRemap(bool set){
	if(set)
	sendCommand(SSD1306_SEGREMAP + 1);
	else
	sendCommand(SSD1306_SEGREMAP);
}

void OLED_01::setCOMRemap(bool set){
	if(set){
		sendCommand(SSD1306_COMSCANDEC);
	}
	else{
		sendCommand(SSD1306_COMSCANINC);
	}
}

void OLED_01::displayFrame(const uint8_t* frame){
	goToStart();
	for(uint16_t i = 0; i < 1024; i += 1){
		writeToRAM(pgm_read_byte(&frame[i]));
	}
}

void OLED_01::clearDisplay(){
	goToStart();
	for(uint16_t i = 0; i < 1024; i += 1){
		writeToRAM(0);
	}
}

void OLED_01::goToStart(){
	goToLine(0);
	setColumnStart(0);
}

void OLED_01::goToStartOfLine(){
	setColumnStart(0);
}

void OLED_01::goToColumn(uint8_t col){
	clearLineAhead(col);
	setColumnStart(col);
}

uint16_t OLED_01::getColumn(){
	return Cptr;
}

uint16_t OLED_01::getRow(){
	return Rptr;
}

void OLED_01::enableInvertMode(bool invert){
	InvertMode = invert;
}

bool OLED_01::isInverted(){
	return Inverted;
}

bool OLED_01::isInInvertMode(){
	return InvertMode;
}

uint8_t OLED_01::getAddress(){
	return addr;
}

void OLED_01::goToNextLine(){
	Rptr += 1;
	sendCommand(0xB0 | Rptr);
}

void OLED_01::goBackALine(){
	Rptr -= 1;
	goToLine(Rptr);
}

void OLED_01::setFontSize(uint8_t fsize){
	FontSize = fsize;
	if(fsize == 2)
	FontPtr = Tahoma13x14;
	if(fsize == 4)
	FontPtr = Tahoma15x32;
}

void OLED_01::printChar(uint8_t data){
	if(isI2CInitialized()){
		switch(FontSize){
			case 0:
			case 1:
			printCharSmall(data);
			break;
			case 2:
			case 4:
			printCharMedium(data);
			break;
		}
	}
}

void OLED_01::printCharSmall(uint8_t data){
	static uint16_t fontptr;
	if(data == '\n' && Old_Sdata == '\r'){
		goToNextLine();
		goToStartOfLine();
	}
	Old_Sdata = data;
	if(data == '\n' || data == '\r' || data < 0x20 || data > 0x7F){
		return;
	}
	else{
		fontptr = data - 0x20;
		fontptr *= 5;
		for (uint8_t i = 0; i < 5; i += 1){
			writeToRAM(pgm_read_byte(&font_data[fontptr]));
			fontptr += 1;
		}
		writeToRAM(0);
	}
}

void OLED_01::printCharMedium(uint8_t data){
	if(data == '\n' && Old_Sdata == '\r'){
		for(uint8_t i = 0; i < FontSize; i += 1){
			goToNextLine();
		}
		goToStartOfLine();
	}
	Old_Sdata = data;
	if(data == '\n' || data == '\r' || data < 0x20 || data > 0x7F){
		return;
	}
	else{
		RowSave = Rptr;
		ColSave = Cptr;
		if(FontSize == 2){
			fontptr = data - 0x20;
			fontptr *= 27;
		}
		if(FontSize == 4){
			fontptr = data - 0x30;
			fontptr *= 61;
		}
		CharLength = pgm_read_byte(&FontPtr[fontptr]);
		CharLengthPtr = fontptr;
		for(uint8_t j = 1; j <= FontSize; j += 1){
			setColumnStart(ColSave);
			fontptr = CharLengthPtr;
			fontptr += j;
			for(uint8_t i = 0; i < CharLength; i += 1){
				if(Cptr != SSD1306_LCDWIDTH - 1 && isI2CGood()){
					if(InvertMode){
						writeToRAM(~(pgm_read_byte(&FontPtr[fontptr])));
					}
					else{
						writeToRAM(pgm_read_byte(&FontPtr[fontptr]));
					}
				}
				fontptr += FontSize;
			}
			if(Cptr < SSD1306_LCDWIDTH - 4 && isI2CGood()){
				if(InvertMode){
					writeToRAM(0xFF);
					writeToRAM(0xFF);
				}
				else{
					writeToRAM(0);
					writeToRAM(0);
				}
			}
			goToNextLine();
		}
		goToLine(RowSave);
	}
}
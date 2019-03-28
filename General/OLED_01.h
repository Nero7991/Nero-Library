/*
 * OLED_01.h
 *
 * Created: 9/5/2017 7:26:22 PM
 *  Author: orencollaco
 */ 


#ifndef OLED_01_H_
#define OLED_01_H_

#include "Essential.h"
#include "StringN.h"
#include "Font_5x7.h"
#include "Century_Gotholic14x16.h"
#include "Tahoma23x27.h"

//Commands

#define SET_CONTRAST				0x81
#define DISPLAY_ON					0xAF
#define DISPLAY_OFF					0xAE
#define ENTIRE_DISPLAY_ON			0xA5
#define DISPLAY_GDDR				0xA4
#define DISPLAY_INVERTED			0xA7
#define DISPLAY_NOT_INVERTED		0xA6

#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_PAGESTART  0xB0

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
		
//Addressing Modes
#define HORIZONTAL_ADDRESSING		0x00
#define VERTICAL_ADDRESSING			0x01
#define PAGE_ADDRESSING				0x02

class OLED_01 : public StringN{
	
private:
	 
	 uint8_t addr, Cbyte,  Rptr, Cptr, Addr_Mode, Old_Sdata, FontSize;
	 uint16_t ptr;
	 uint8_t ColSave, RowSave, CharLength;
	 uint16_t fontptr, CharLengthPtr;
	 const uint8_t *FontPtr;
	 bool Inverted, InvertMode, PowerON;

public:
	
	OLED_01();
	
	OLED_01(uint8_t Addr);
	
	OLED_01(uint8_t Adddr, uint8_t Addr_Mode);
	
	void initialize();
	
	void setDeviceAddress(uint8_t Addr);
	
	uint8_t	scanI2CDevices();
	
	void ssd1306_command(uint8_t cmd);
	
	void sendCommand(uint8_t cmd);
	
	uint8_t readStatus();
	
	void setContrastTo(uint8_t contrast);
	
	void enablePower(bool set);
	
	bool getPowerEnabled();
	
	void setEntireDisplayOn();
	
	void setDisplayToGDDRAM();
	
	void invertDisplay(bool set);
	
	void setAddressingMode(uint8_t mode);
	
	void setPageAddress(uint8_t start, uint8_t stop);
	
	void goToLine(uint8_t start);
	
	void setColumnStart(uint8_t start);
	
	void goToColumn(uint8_t col);
	
	uint16_t getColumn();
	
	uint16_t getRow();
	
	void setColumnAddress(uint8_t start, uint8_t stop);
	
	void testOLED();
	
	void writeToRAM(uint8_t data);
	
	void setStartLine(uint8_t data);
	
	void setSegmentRemap(bool set);
	
	void setCOMRemap(bool set);
	
	void displayFrame(const uint8_t* frame);
	
	bool isInverted();
	
	uint8_t getAddress();
	
	void goToStart();
	
	void goBackALine();
	
	void goToNextLine();
	
	void goToStartOfLine();
	
	void printCharSmall(uint8_t data);
	
	void printCharMedium(uint8_t data);
	
	void clearDisplay();
	
	void clearLineAhead();
	
	void clearLineAhead(uint8_t column);
	
	void setFontSize(uint8_t fsize);
	
	void printChar(uint8_t data);
	
	void enableInvertMode(bool invert);
	
	bool isInInvertMode();
	
	};




#endif /* OLED_01_H_ */
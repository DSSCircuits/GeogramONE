#include <I2C.h>

#ifndef BMA250_h
#define BMA250_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#define BMA_ADD    (uint8_t)0x18
#define DELAY               64000

#define BW7P81              0x08 //7.81Hz bandwith
#define BW15P63             0x09 //15.63Hz bandwith
#define BW31P25             0x0A //31.25Hz bandwith
#define BW62P5              0x0B //62.50Hz bandwith
#define BW125		        0x0C //62.50Hz bandwith
#define BW250               0x0D //62.50Hz bandwith
#define BW500               0x0E //62.50Hz bandwith
#define BW1000              0x0F //62.50Hz bandwith


#define GSEL2               0x03 //0x03 - 2g, 0x05 - 4, 0x08 - 8g, 0x0C - 16g
#define GSEL4				0x05 //0x03 - 2g, 0x05 - 4, 0x08 - 8g, 0x0C - 16g
#define GSEL8				0x08 //0x03 - 2g, 0x05 - 4, 0x08 - 8g, 0x0C - 16g
#define GSEL16				0x0C //0x03 - 2g, 0x05 - 4, 0x08 - 8g, 0x0C - 16g

struct registersBMA250
{
	uint8_t zeroF;
	uint8_t oneZero;
	uint8_t oneOne;
	uint8_t oneSix;
	uint8_t oneSeven;
	uint8_t oneNine;
	uint8_t oneA;
	uint8_t oneB;
	uint8_t twoZero;
	uint8_t twoOne;
	uint8_t twoFive;
	uint8_t twoSix;
	uint8_t twoSeven;
	uint8_t twoEight;
};

class BMA250
{
	public:
		BMA250(I2C *iic);
		void init(uint8_t, uint16_t);
		void configureMotion();
		void configureInterrupts();
		void enableInterrupts();
		void disableInterrupts();
		registersBMA250 config;
	private:
		I2C *i2c;
};
#endif

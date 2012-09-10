#include "BMA250.h"

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
    return i;
}

BMA250::BMA250(I2C *iic)
{
	i2c = iic;
}

uint8_t BMA250::init(uint8_t pinNum, uint16_t tOut)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
	i2c->begin();
	i2c->timeOut(tOut);
	i2c->write(BMA_ADD,0x0F,GSEL4);
	//i2c->write(BMA_ADD,0x10,BW7P81);
	//i2c->write(BMA_ADD,0x10,BW1000);
	//i2c->write(BMA_ADD,0x19,0x40);  //int1
	//i2c->write(BMA_ADD,0x1B,0x04);  //map to INT2
	//i2c->write(BMA_ADD,0x20,0x02); // set int1 to Open Drain active low, int2 to PP active low
	//i2c->write(BMA_ADD,0x11,0x5F); //set lower power mode to 1s
	return(0);
}

uint8_t BMA250::activateMotionDetection()
{
	
	i2c->write(BMA_ADD,0x21,0x8E);  //set interrupts to latched  8F
	delayMicroseconds(600);
	i2c->write(BMA_ADD,0x16,0x40);  //enable interrupts
	return(0);
}

uint8_t BMA250::highG()
{
	i2c->write(BMA_ADD,0x26,0x00);  //set high threshold to 7.8mg
	i2c->write(BMA_ADD,0x25,0x00);  //set time duration to 2ms
	i2c->write(BMA_ADD,0x19,0x02);  //map to INT1
	delayMicroseconds(600);
	i2c->write(BMA_ADD,0x17,0x07);  //enable high g for x, y and z
	return(0);
}

uint8_t BMA250::anyMotion()
{
	i2c->write(BMA_ADD,0x0F,GSEL4);
	i2c->write(BMA_ADD,0x10,BW7P81);
	i2c->write(BMA_ADD,0x28,0x05);  //set slope threshold to 19.55mg
	i2c->write(BMA_ADD,0x27,0x00);  //set slope duration to 1 bits
	i2c->write(BMA_ADD,0x20,0x02); // set int1 to Open Drain active low, int2 to PP active low
	i2c->write(BMA_ADD,0x19,0x04);  //map to INT1
	//i2c->write(BMA_ADD,0x1B,0x04);  //map to INT2
	i2c->write(BMA_ADD,0x21,0x8E);  //set interrupts to not latched  50ms
	delayMicroseconds(600);
	i2c->write(BMA_ADD,0x16,0x07);  //enable high g for x, y and z
	return(0);
}

uint8_t BMA250::disableInterrupts()
{
	i2c->write(BMA_ADD,0x16,0x00);  //enable high g for x, y and z
}

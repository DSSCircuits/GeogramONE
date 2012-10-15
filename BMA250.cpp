#include "BMA250.h"

BMA250::BMA250(I2C *iic)
{
	i2c = iic;
}

void BMA250::init(uint8_t pinNum, uint16_t tOut)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
	i2c->begin();
	i2c->timeOut(tOut);
	configureMotion();
	configureInterrupts();
}

void BMA250::configureMotion()
{
	i2c->write(BMA_ADD,(uint8_t)0x11,config.oneOne);
	i2c->write(BMA_ADD,(uint8_t)0x25,config.twoFive);
	i2c->write(BMA_ADD,(uint8_t)0x26,config.twoSix);
	i2c->write(BMA_ADD,(uint8_t)0x0F,config.zeroF);     //GSEL4
	i2c->write(BMA_ADD,(uint8_t)0x10,config.oneZero);   //BW7P81
	i2c->write(BMA_ADD,(uint8_t)0x27,config.twoSeven);  //set slope duration to 1 bits
	i2c->write(BMA_ADD,(uint8_t)0x28,config.twoEight);  //set slope threshold to 19.55mg	
}

void BMA250::configureInterrupts()
{
	i2c->write(BMA_ADD,(uint8_t)0x11,config.oneOne);
	i2c->write(BMA_ADD,(uint8_t)0x25,config.twoFive);
	i2c->write(BMA_ADD,(uint8_t)0x26,config.twoSix);
	i2c->write(BMA_ADD,(uint8_t)0x19,config.oneNine);  	//map to INT1
	i2c->write(BMA_ADD,(uint8_t)0x1A,config.oneA);     	//map to INT1/INT2
	i2c->write(BMA_ADD,(uint8_t)0x1B,config.oneB);		//map to INT2
	i2c->write(BMA_ADD,(uint8_t)0x20,config.twoZero);  	//INT drive signal
}

void BMA250::enableInterrupts()
{
	i2c->write(BMA_ADD,(uint8_t)0x21,config.twoOne);  	//INT latching 0x8E
	delayMicroseconds(600);
	i2c->write(BMA_ADD,(uint8_t)0x16,config.oneSix);  	//enable INT1 0x07
	i2c->write(BMA_ADD,(uint8_t)0x17,config.oneSeven);  //enable INT1
}

void BMA250::disableInterrupts()
{
	i2c->write(BMA_ADD,(uint8_t)0x16,(uint8_t)0x00);  	//disable Interrupts
	i2c->write(BMA_ADD,(uint8_t)0x17,(uint8_t)0x00);  	//disable Interrupts
}


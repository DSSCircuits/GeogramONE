#include "MAX17043.h"



MAX17043::MAX17043(I2C *iic)
{
	i2c = iic;
}


void MAX17043::init(uint8_t pinNum, uint16_t tOut)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
	i2c->begin();
	i2c->timeOut(tOut);
	quickStart();
	setAlertLevel();
	clearAlertFlag();
	return;
}

uint8_t MAX17043::quickStart()
{	/*initiate a quick start*/
	uint8_t fgData[2] = {0x40,0x00}; //MSB is transmitted first
	return(i2c->write(FUELGAUGE,0x06,fgData,2));
}

uint8_t MAX17043::setAlertLevel()   //saves as actual percentage
{
	uint8_t configRegister[2] = {0,0};
	batteryInterruptValue -= 32;
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	batteryInterruptValue &= 0x1F;  // set 3msb to zero
	configRegister[1] &= 0xE0;  // zero battery level  , was originally FE
	configRegister[1] |= batteryInterruptValue; //set new battery level
	return(i2c->write(FUELGAUGE,0x0C,configRegister,2));
}

void MAX17043::configureBatteryAlert(uint8_t setAlert)
{
	if(!setAlert) //we don't want to use the low battery alert function
	{
		pinMode(FUELGAUGEPIN,INPUT);
		digitalWrite(FUELGAUGEPIN,LOW);  //set pin to high impedance
	}
	if(setAlert) //we do want to use the low battery alert function
	{
		pinMode(FUELGAUGEPIN,INPUT);
		digitalWrite(FUELGAUGEPIN,HIGH);
	}
}

uint8_t MAX17043::clearAlertFlag()
{
	uint8_t configRegister[2] = {0,0};
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	configRegister[1] &= 0xDF;
	return(i2c->write(FUELGAUGE,0x0C,configRegister,2));
}

uint8_t MAX17043::getAlertFlag()
{
	uint8_t configRegister[2] = {0,0};
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	configRegister[1] &= 0xDF;
	return((configRegister[1] &= 0xDF) >> 5);
}

uint16_t MAX17043::getBatterySOC()
{
	uint16_t batterySOC = 0;
	i2c->read(FUELGAUGE,0x04,2);
	for(uint8_t i = 0;i < 2;i++)
	{ 
		batterySOC <<= 8;
		batterySOC |= i2c->receive();
	}
	return(map(batterySOC,0x0000,0x6400,0,10000));
}

uint16_t MAX17043::getBatteryVoltage()
{
	uint16_t batteryVoltage = 0;
	i2c->read(FUELGAUGE,0x02,2);
	for(uint8_t i = 0;i < 2;i++)
	{ 
		batteryVoltage <<= 8;
		batteryVoltage |= i2c->receive();
	}
	return(batteryVoltage >>= 4);
}

uint8_t MAX17043::sleepFuelGauge()
{
	uint8_t configRegister[2] = {0,0};
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	configRegister[1] |= 0x80;
	return(i2c->write(FUELGAUGE,0x0C,configRegister,2));
}

uint8_t MAX17043::wakeFuelGauge()
{
	uint8_t configRegister[2] = {0,0};
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	configRegister[1] &= 0x7F;
	return(i2c->write(FUELGAUGE,0x0C,configRegister,2));
}

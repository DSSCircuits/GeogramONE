#include "MAX17043.h"

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


MAX17043::MAX17043(I2C *iic)
{
	i2c = iic;
}


uint8_t MAX17043::init(uint8_t pinNum, uint16_t tOut)
{
/*EEPROM_writeAnything(BATTERYINTONOFF,(uint8_t)0x01);
  EEPROM_writeAnything(BATTERYLOWLEVEL,(uint8_t)32);*/

	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
	i2c->begin();
	i2c->timeOut(tOut);
	uint8_t fgData[2] = {0x40,0x00}; //MSB is transmitted first
	/*initiate a quick start*/
	if(i2c->write(0x36,0x06,fgData,2))
		return 1;
	return 0;
}



uint8_t MAX17043::initializeFuelGauge()
{
	uint8_t fgData[2] = {0x40,0x00}; //MSB is transmitted first
	/*initiate a quick start*/
	if(i2c->write(FUELGAUGE,0x06,fgData,2))
		return 1;
	if(!batteryAlert())  //check if fuel gauge alert is used
	{
		configureBatteryAlert(0); // not used, disable interrupt
		return 0;
	}
	setAlertLevel(); //set alert level to saved setting in EEPROM
	configureBatteryAlert(1);
	return 0;
}

uint8_t MAX17043::setAlertLevel(uint8_t batteryLevel)   //saves as actual percentage
{
	uint8_t configRegister[2] = {0,0};
	if(batteryLevel >= 33)
		return(1); 
	if(!batteryLevel)
		EEPROM_readAnything(BATTERYLOWLEVEL,batteryLevel);
	batteryLevel = 32 - batteryLevel;
	i2c->read(FUELGAUGE,0x0C,2,configRegister);
	batteryLevel &= 0x1F;  // set 3msb to zero
	configRegister[1] &= 0xE0;  // zero battery level  , was originally FE
	configRegister[1] |= batteryLevel; //set new battery level
	return(i2c->write(FUELGAUGE,0x0C,configRegister,2));
}

uint8_t MAX17043::batteryAlert(uint8_t setAlert) // going to do away with this
{
	if(!setAlert)
	{
		EEPROM_readAnything(BATTERYINTONOFF,setAlert);
		return(setAlert & 0x01);
	}
	if(setAlert == 1)
	{
		EEPROM_writeAnything(BATTERYINTONOFF,setAlert);
		return 1;
	}
	if(setAlert == 2)
	{
		EEPROM_writeAnything(BATTERYINTONOFF,(uint8_t)0x00);
		return 0;
	}
}

void MAX17043::configureBatteryAlert(uint8_t setAlert)
{
	if(!setAlert) //we don't want to use the low battery alert function
	{
		//PCintPort::detachInterrupt(FUELGAUGEPIN);  //need to revisit
		pinMode(FUELGAUGEPIN,INPUT);
		digitalWrite(FUELGAUGEPIN,LOW);  //set pin to high impedance
	}
	if(setAlert) //we do want to use the low battery alert function
	{
		pinMode(FUELGAUGEPIN,INPUT);
		digitalWrite(FUELGAUGEPIN,HIGH);
		//PCintPort::attachInterrupt(FUELGAUGEPIN, &lowBattery, FALLING);  //need to revisit
	}
}

uint8_t MAX17043::saveAlertLevel(uint8_t intLevel)    //going to do away with this
{
	if((intLevel == 0) || (intLevel >= 33))
		return 1; // level must be between 1 and 32%
	EEPROM_writeAnything(BATTERYLOWLEVEL,32-intLevel);
	return 0;
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

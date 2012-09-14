#include <I2C.h>

#ifndef MAX17043_h
#define MAX17043_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define FUELGAUGE          		0x36 //Fuel gauge I2C address
#define FUELGAUGEALERT     		0x11 //set to 15% battery capacity
#define FUELGAUGEPIN       		0x07 //Fuel gauge interrupt pin



class MAX17043 
{
	public:
		MAX17043(I2C *iic);
		void init(uint8_t, uint16_t);
		uint8_t quickStart();
		uint8_t setAlertLevel();
		uint8_t clearAlertFlag();
		uint8_t getAlertFlag();
		void configureBatteryAlert(uint8_t);
		uint16_t getBatterySOC();
		uint16_t getBatteryVoltage();
		uint8_t sleepFuelGauge();
		uint8_t wakeFuelGauge();
		uint8_t batteryInterruptValue;
	private:
		I2C *i2c;
};

#endif
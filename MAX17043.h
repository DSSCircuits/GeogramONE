#include <I2C.h>
#include <EEPROM.h>

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
#define BATTERYLOWLEVEL    		88
#define BATTERYINTONOFF    		89

class MAX17043
{
	public:
		MAX17043(I2C *iic);
		uint8_t init(uint8_t, uint16_t);
		uint8_t initializeFuelGauge();
		uint8_t setAlertLevel(uint8_t batteryLevel = 0);
		uint8_t saveAlertLevel(uint8_t intLevel);
		uint8_t clearAlertFlag();
		uint8_t getAlertFlag();
		void configureBatteryAlert(uint8_t);
		uint8_t batteryAlert(uint8_t setAlert = 0);
		uint16_t getBatterySOC();
		uint16_t getBatteryVoltage();
		uint8_t sleepFuelGauge();
		uint8_t wakeFuelGauge();
	private:
		I2C *i2c;
};

#endif
#ifndef PA6C_h
#define PA6C_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define GPSTIMEOUT			1100
#define KNOTSTOMPH			1.15078
#define KNOTSTOKPH			1.852
#define METERSTOFEET		3.2808

struct gpsData
{
	uint8_t positionFixInd;
	uint8_t satellitesUsed;
	float altitude;
	
	char mode1;
	uint8_t mode2;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	
	uint8_t seconds;
	uint8_t minute;
	uint8_t hour;
	char rmcStatus;
	long latitude;
	long longitude;
	uint16_t speedKnots;
	uint16_t course;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	
/*****************************/	
	char amPM;
	char courseDirection[3];
	float speedKmr;
	char rmcMode;
	
};

class PA6C
{
	public:
		PA6C(HardwareSerial *ser);
		uint8_t getTheData2(gpsData *);
		uint8_t init(unsigned long);
		uint8_t sleepGPS();
		uint8_t wakeUpGPS();
	private:
		void filterData(char *, gpsData *, uint8_t *, uint8_t *);
		uint8_t saveCoordinates(gpsData *);
		void directionOfTravel(gpsData *);
		HardwareSerial *gpsSerial;
};

#endif
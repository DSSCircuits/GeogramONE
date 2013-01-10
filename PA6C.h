#include <avr/pgmspace.h>
#include <EEPROM.h>

#ifndef PA6C_h
#define PA6C_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define GPRMC   			"GPRMC"
#define GPGGA   			"GPGGA"
#define GPGSA   			"GPGSA"
#define GPVTG   			"GPVTG"
#define GPGSV   			"GPGSV"
#define PMTKLOG 			"PMTKLOG"
#define PMTK001 			"PMTK001"
#define PMTK869 			"PMTK869"
#define PMTKLOX 			"PMTKLOX"

#define USEHDOP				1
#define USEVDOP				1
#define USEPDOP				1
#define USEALTITUDE			1
#define USESPEEDKNOTS		1
#define USECOURSE			1
#define USEMODE2			1
#define USESATINVIEW		0
#define USESATELLITESUSED	1
#define USEPOSITIONFIXIND	1
#define USEGSAMODE1			1
#define USEVTGMODE			1
#define USERMCMODE			1
#define USERMCSTATUS		1

#define GPSTIMEOUT			1100

#define KNOTSTOMPH			1.15078
#define KNOTSTOKPH			1.852
#define METERSTOFEET		3.2808


struct geoFence
{
	uint8_t inOut;
	uint8_t reset;
	long latitude;
	long longitude;
	unsigned long radius;
};

struct configVar
{
	bool engMetric;
	uint8_t timeFormat;
	int8_t timeZone;
};


struct time
{
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	char amPM;
};
struct date
{
	uint8_t month;
	uint8_t day;
	uint8_t year;
};


struct gpsData
{
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	char amPM;
	uint8_t month;
	uint8_t day;
	uint16_t year;
	long latitude;
	long longitude;
	float altitude;
	uint16_t course;
	char courseDirection[3];
	uint16_t speedKnots;
	uint16_t hdop;
	uint16_t vdop;
	uint16_t pdop;
	uint8_t mode2;
    uint8_t satellitesUsed;
	uint8_t positionFixInd;
	char gsaMode1;
	float speedKmr;
	char rmcMode;
	char rmcStatus;
};

class PA6C
{
	public:
		PA6C(HardwareSerial *ser);
		configVar settings;
		uint8_t getTheData(gpsData *);
		uint8_t getTheData2(gpsData *);
		uint8_t init(unsigned long);
		uint8_t sleepGPS();
		uint8_t wakeUpGPS();
		uint8_t geoFenceDistance(gpsData *, geoFence *);
	private:
		struct GPS
		{
			char field[15];
			uint8_t checksum;
			uint8_t index;
			uint8_t returnStatus;
			unsigned long timeOut;
			uint8_t dataCollected;
		} gps;
		struct pmtkCommand
		{
			uint8_t p001Cmd;
			uint8_t p001Flag;
		} pmtk001;
		uint8_t filterData(char *, gpsData *, uint8_t *, uint8_t *)
/*		uint8_t getGPRMC(GPS *, gpsData *);
		uint8_t getGPGSV(GPS *, gpsData *);
		uint8_t getGPGGA(GPS *, gpsData *);
		uint8_t getGPGSA(GPS *, gpsData *);
		uint8_t nextField(GPS *);
		uint8_t verifyChecksum(GPS *);
		void lookForDollarSign(GPS *);
		void getSentenceId(GPS *, char *);*/
		uint8_t getPMTK001(GPS *);
		uint8_t saveCoordinates(gpsData *);
		void directionOfTravel(gpsData *);
		HardwareSerial *gpsSerial;
};

#endif
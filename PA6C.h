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
#define USEVDOP				0
#define USEPDOP				1
#define USEALTITUDE			0
#define USESPEEDKNOTS		1
#define USECOURSE			1
#define USEMODE2			1
#define USESATINVIEW		0
#define USESATELLITESUSED	1
#define USEPOSITIONFIXIND	0
#define USEGSAMODE1			0
#define USEVTGMODE			0
#define USESPEEDKMR			0
#define USERMCMODE			0
#define USERMCSTATUS		0

#define GPSTIMEOUT			1100
#define TIMEZONE			-4
#define MPHORKPH			1	//0 - use kph, 1 - use mph
#define KNOTSTOMPH			1.15078
#define KNOTSTOKPH			1.852
#define METERSTOFEET		3.2808

//#define NEWTIME				0
#define AMPM				0   // 0 - am/pm,  1 - 24 hour format

#define ACTIVE1					100
#define ACTIVE2					114
#define ACTIVE3					128
#define RESET1					101
#define RESET2					116
#define RESET3					131
#define INOUT1					102
#define INOUT2					117
#define INOUT3					132
#define RADIUS1					103
#define RADIUS2					118
#define RADIUS3					133
#define LATITUDE1				107
#define LATITUDE2				122
#define LATITUDE3				137
#define LONGITUDE1				111
#define LONGITUDE2				126
#define LONGITUDE3				141

//Fence algortithm parameters
#define BREACHREPS				10
#define BREACHSPEED				2


struct geoFence
{
	uint8_t inOut;
	uint8_t reset;
	float latitude;
	float longitude;
	unsigned long radius;
};

struct time
{
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	char amPM[1];
};
struct date
{
	uint8_t month;
	uint8_t day;
	uint8_t year;
};


struct gpsData
{
//#if NEWTIME
//	long utcTime;
//#else
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	char amPM[1];
//#endif
//#if NEWTIME
//	long date;
//#else
	uint8_t month;
	uint8_t day;
	uint16_t year;
//#endif
	long latitude;
	long longitude;
#if USEALTITUDE
	float altitude;
#endif
#if USECOURSE
	uint16_t course;
	char courseDirection[3];
#endif
#if USESPEEDKNOTS	
	uint16_t speedKnots;
#endif
#if USEHDOP
	uint16_t hdop;
#endif
#if USEVDOP	
	uint16_t vdop;
#endif
#if USEPDOP
	uint16_t pdop;
#endif
#if USEMODE2
	uint8_t mode2;
#endif
#if USESATINVIEW
	uint8_t satInView;
#endif
#if USESATELLITESUSED
    uint8_t satellitesUsed;
#endif
#if USEPOSITIONFIXIND
	uint8_t positionFixInd;
#endif
#if USEGSAMODE1
	char gsaMode1[2];
#endif
#if USEVTGMODE
	char vtgMode[2];
#endif
#if USESPEEDKMR
	float speedKmr;
#endif
#if USERMCMODE
	char rmcMode[2];
#endif
#if USERMCSTATUS
	char rmcStatus[2];
#endif
};

class PA6C
{
	public:
		PA6C(HardwareSerial *ser);
		uint8_t getTheData(gpsData *);
		uint8_t init(unsigned long);
		uint8_t sleepGPS();
		uint8_t wakeUpGPS();
		uint8_t geoFenceDistance(gpsData *, geoFence *);
		uint8_t configureFence(uint8_t, geoFence *);
		uint8_t isFenceActive(uint8_t fenceNumber, uint8_t *fenceVar);
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
		uint8_t getGPRMC(GPS *, gpsData *);
		uint8_t getGPGSV(GPS *, gpsData *);
		uint8_t getGPGGA(GPS *, gpsData *);
		uint8_t getGPGSA(GPS *, gpsData *);
		uint8_t getPMTK001(GPS *);
		uint8_t nextField(GPS *);
		uint8_t verifyChecksum(GPS *);
		uint8_t saveCoordinates(gpsData *);
		void lookForDollarSign(GPS *);
		void getSentenceId(GPS *, char *);
	#if USECOURSE
		void directionOfTravel(gpsData *);
	#endif
		HardwareSerial *gpsSerial;
};

#endif
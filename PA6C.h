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

#define GPGGA				"GA"
#define GPGSA				"SA"
#define GPGSV				"SV"
#define GPRMC				"MC"
#define GPVTG				"TG"

#define PMTK161				"$PMTK161,0*28"
#define PMTK000				"$PMTK000*32"
				
struct gpsData
{
/***GPGGA Variables**********/	
	uint8_t positionFixInd;
	uint8_t satellitesUsed;
	float altitude;
	
/***GPGSA Variables**********/		
	char mode1;
	uint8_t mode2;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;

/***GPRMC Variables**********/	
	uint8_t seconds;
	uint8_t minute;
	int8_t hour;
	char rmcStatus;
	long latitude;
	long longitude;
	uint16_t speed;
	uint16_t course;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	char amPM;
	char courseDirection[3];
	/*GPS Trace Orange*/
	float orangeLat;
	float orangeLon;
	long orangeAltitude;
	long orangeDate;
	long orangeTime;
	int orangeCourse;
	long orangeSpeed;
	char orangeNS;
	char orangeEW;
	bool signalLock;
	
/*****************************/
	
};

struct goCoord
{
	char latitude[10];
	char longitude[11];
	char ns;
	char ew;
	char time[7];
	char date[7];
//	char rmcStatus; //use signalLock instead
	uint16_t speed;
	uint16_t course;
	uint8_t satellitesUsed;
	float altitude;
	bool signalLock;
	uint8_t updated;
};

struct geoFence
{
	uint8_t inOut;
	uint8_t reset;
	long latitude;
	long longitude;
	unsigned long radius;
};

class PA6C
{
	public:
		PA6C(HardwareSerial *ser);
		uint8_t getCoordinates(gpsData *);
		uint8_t init(unsigned long);
		uint8_t sleepGPS();
		uint8_t wakeUpGPS();
		void customConfig(int8_t, bool, uint8_t, bool);
		uint8_t geoFenceDistance(gpsData *, geoFence *);
	private:
		bool amPMFormat;
		int8_t timeZoneUTC;
		uint8_t speedType;
		bool impMetric;
		void filterData(char *, gpsData *, uint8_t *, uint8_t *);
		void directionOfTravel(gpsData *);
		HardwareSerial *gpsSerial;
};

#endif
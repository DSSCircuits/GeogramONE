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
				

struct goCoord
{
	char latitude[10];
	char longitude[11];
	char ns;
	char ew;
	char time[7];
	char date[7];
	uint8_t positionFixInd;
	uint8_t mode2;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	uint32_t speed;
	uint16_t course;
	char courseDirection[3];
	int satellitesUsed;
	long altitude;
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
		uint8_t getCoordinates(goCoord *);
		uint8_t init(unsigned long);
		uint8_t sleepGPS();
		uint8_t wakeUpGPS();
		uint8_t geoFenceDistance(goCoord *, geoFence *);
	private:
//		bool amPMFormat;
//		int8_t timeZoneUTC;
//		uint8_t speedType;
//		bool impMetric;
		void filterData(char *, goCoord *, uint8_t *, uint8_t *);
		void directionOfTravel(goCoord *);
		HardwareSerial *gpsSerial;
};

#endif
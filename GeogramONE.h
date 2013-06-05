#include <avr/sleep.h>
#include <EEPROM.h>
#include "SIM900.h"
#include "PA6C.h"

#ifndef GeogramONE_h
#define GeogramONE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/*******EEPROM ADDRESSES**********/
#define PINCODE         		0
#define SMSADDRESS				5
#define RETURNADDCONFIG			44
#define TIMEZONE				45   //use -4 for EST
#define DATEFORMAT				46   // 0 - mm/dd/yy , 1 - yy/mm/dd
#define ENGMETRIC				47   // 0 - English (mph, ft, etc...), 1 = Metric (kph, m, etc...)
#define GEODATAFORMAT1			48
#define GEODATAFORMAT2			50
#define BATTERYLOWLEVEL    		52
#define SMSSENDINTERVAL			53
#define IOSTATE0				57
#define IOSTATE1				58
#define IOSTATE2				59
#define IOSTATE3				60
#define IOSTATE4				61
#define IOSTATE5				62
#define IOSTATE6				63
#define IOSTATE7				64
#define IOSTATE8				65
#define IOSTATE9				66
#define SLEEPTIMECONFIG			68
#define SLEEPTIMEON				69
#define SLEEPTIMEOFF			73
#define SPEEDLIMIT				77
#define SPEEDHYST				79
#define ACTIVE1					80
#define INOUT1					81
#define RADIUS1					82
#define LATITUDE1				86
#define LONGITUDE1				90
#define ACTIVE2					94
#define INOUT2					95
#define RADIUS2					96
#define LATITUDE2				100
#define LONGITUDE2				104
#define ACTIVE3					108
#define INOUT3					109
#define RADIUS3					110
#define LATITUDE3				114
#define LONGITUDE3				118
#define BREACHSPEED				122
#define BREACHREPS				123
#define BMA0X0F					124
#define BMA0X10					125
#define BMA0X11					126
#define BMA0X16					127
#define BMA0X17					128
#define BMA0X19					129
#define BMA0X1A					130
#define BMA0X1B					131
#define BMA0X20					132
#define BMA0X21					133
#define BMA0X25					134
#define BMA0X26					135
#define BMA0X27					136
#define BMA0X28					137
#define UDPSENDINTERVAL			138
#define MOTIONMSG				200
#define BATTERYMSG				225
#define FENCE1MSG				250
#define FENCE2MSG				275
#define FENCE3MSG				300
#define SPEEDMSG				325
#define MAXSPEEDMSG				350
#define GEOGRAMONEID			375
#define D4MSG					400
#define D10MSG					425
#define HTTP1					450
#define HTTP2					500
#define HTTP3					550
#define IMEI					600
#define GPRS_APN				616
#define GPRS_USER				666
#define GPRS_PASS				691
#define GPRS_HOST				716
#define GPRS_PORT				732
#define UDP_HEADER				734
#define UDP_REPLY				745


/**********************************/

#define PG_INT             		14
#define BMA_ADD    	   (uint8_t)0x18
#define FUELGAUGE          		0x36 //Fuel gauge I2C address
#define FUELGAUGEALERT     		0x11 //set to 15% battery capacity
#define FUELGAUGEPIN       		0x07 //Fuel gauge interrupt pin


class GeogramONE 
{
	public:
		GeogramONE( );
		void init( );
		void goToSleep();
		void configureIO( uint8_t, uint8_t );
		void configureFence(uint8_t , geoFence *);
		void configureBreachParameters(uint8_t *, uint8_t *);
		void getFenceActive(uint8_t fenceNumber, uint8_t *fenceVar);
		void configureInterval(uint32_t *, uint32_t *, uint32_t *, uint8_t *, uint32_t *);
		void configureSpeed(uint8_t *, uint8_t *, uint16_t *);
	private:
};
#endif

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




#define TIMEZONE				88   //use -4 for EST
#define TIMEFORMAT				89   // bit 0: 0 - am/pm,  1 - 24 hour format  bit 1: 0 - mm/dd/yy , 1 - yy/mm/dd
#define ENGMETRIC				90   // 0 - English (mph, ft, etc...), 1 = Metric (kph, m, etc...)
#define GEODATAFORMAT1			91
#define GEODATAFORMAT2			93
#define BATTERYLOWLEVEL    		95
#define SENDINTERVAL			96
#define IOSTATE0				100
#define IOSTATE1				101
#define IOSTATE2				102
#define IOSTATE3				103
#define IOSTATE4				104
#define IOSTATE5				105
#define IOSTATE6				106
#define IOSTATE7				107
#define IOSTATE8				108
#define IOSTATE9				109
#define SLEEPTIMECONFIG			110
#define SLEEPTIMEON				111
#define SLEEPTIMEOFF			115
#define SPEEDLIMIT				119
#define SPEEDHYST				121
#define ACTIVE1					122
#define INOUT1					123
#define RADIUS1					124
#define LATITUDE1				128
#define LONGITUDE1				132
#define ACTIVE2					136
#define INOUT2					137
#define RADIUS2					138
#define LATITUDE2				142
#define LONGITUDE2				146
#define ACTIVE3					150
#define INOUT3					151
#define RADIUS3					152
#define LATITUDE3				156
#define LONGITUDE3				160
#define BREACHSPEED				164
#define BREACHREPS				165
#define BMA0X0F					166
#define BMA0X10					167
#define BMA0X11					168
#define BMA0X16					169
#define BMA0X17					170
#define BMA0X19					171
#define BMA0X1A					172
#define BMA0X1B					173
#define BMA0X20					174
#define BMA0X21					175
#define BMA0X25					176
#define BMA0X26					177
#define BMA0X27					178
#define BMA0X28					179
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
#define GPRS_HEADER				734
#define GPRS_REPLY				745


/**********************************/

#define PG_INT             		14
#define BMA_ADD    (uint8_t)0x18
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
		void configureInterval(uint32_t *, uint32_t *, uint32_t *, uint8_t *);
		void configureSpeed(uint8_t *, uint8_t *, uint16_t *);
	private:
};
#endif

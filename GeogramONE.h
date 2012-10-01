#include <avr/sleep.h>
#include <EEPROM.h>
#include "SIM900.h"
#include "PA6C.h"
#include "MAX17043.h"
#include "BMA250.h"

#ifndef GeogramONE_h
#define GeogramONE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/*******EEPROM ADDRESSES**********/

#define IOSTATE0				90
#define IOSTATE1				91
#define IOSTATE2				92
#define IOSTATE3				93
#define IOSTATE4				94
#define IOSTATE5				95
#define IOSTATE6				96
#define IOSTATE7				97
#define IOSTATE8				98
#define IOSTATE9				99
#define MOTIONMSG				200
#define BATTERYMSG				225
#define FENCE1MSG				250
#define FENCE2MSG				275
#define FENCE3MSG				300
#define SPEEDMSG				325
#define GEOGRAMONEID			350

#define BATTERYLOWLEVEL    		88
#define BATTERYINTONOFF    		89

#define TIMEZONE				147   //use -4 for EST
#define TIMEFORMAT				148   // bit 0: 0 - am/pm,  1 - 24 hour format  bit 1: 0 - mm/dd/yy , 1 - yy/mm/dd
#define ENGMETRIC				149   // 0 - English (mph, ft, etc...), 1 = Metric (kph, m, etc...)

#define ACTIVE1					100
#define ACTIVE2					115
#define ACTIVE3					130
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

#define BMA0X0F					150
#define BMA0X10					151
#define BMA0X11					152
#define BMA0X16					153
#define BMA0X17					154
#define BMA0X19					155
#define BMA0X1A					156
#define BMA0X1B					157
#define BMA0X20					158
#define BMA0X21					159
#define BMA0X25					160
#define BMA0X26					161
#define BMA0X27					162
#define BMA0X28					163

#define SENDINTERVAL			164
#define SLEEPTIMEON				168
#define SLEEPTIMEOFF			172
#define SLEEPTIMECONFIG			176

#define BREACHSPEED				177
#define BREACHREPS				178
#define GEODATAFORMAT			179

#define PG_INT             		14


class GeogramONE 
{
	public:
		GeogramONE( );
		void init( );
		void goToSleep( uint8_t whichMode = 0 );
		void configureIO( uint8_t, uint8_t );
		uint8_t resetTimer2( );
		uint8_t startTimer2( unsigned long );
		void configureMAX17043(uint8_t *);
		void configureBMA250(registersBMA250 *);
		void configurePA6C(configVar *);
		void configureFence(uint8_t , geoFence *);
		void configureBreachParameters(uint8_t *, uint8_t *);
		void getFenceActive(uint8_t fenceNumber, uint8_t *fenceVar);
		void configureInterval(uint32_t *, uint32_t *, uint32_t *, uint8_t *);
	private:
};
#endif

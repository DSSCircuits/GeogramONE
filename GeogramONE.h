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

#define PG_INT             		14


class GeogramONE
{
	public:
		GeogramONE( );
		uint8_t init( );
		void goToSleep( uint8_t whichMode = 0 );
		uint8_t configureIO( uint8_t, uint8_t );
		uint8_t resetTimer2( );
		uint8_t startTimer2( unsigned long );
	private:
};
#endif

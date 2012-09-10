
#include <AltSoftSerial.h>
#include <PinChangeInt.h>
#include "GeogramONE.h"
#include <EEPROM.h>
#include <I2C.h>
#include "eepromAnything.h"

prog_char googlePrefix[] PROGMEM = "http://maps.google.com/maps?q=";  //30 characters
prog_char displayTimeDate[] PROGMEM = "+(";
prog_char googleSuffix[] PROGMEM = ")&z=19"; //Google Earth full zoom

AltSoftSerial GSM;
MAX17043 max17043(&I2c);
SIM900 sim900(&GSM);
BMA250 bma250(&I2c);
geoSmsData smsData;
PA6C gps(&Serial);
gpsData lastValid;
geoFence fence;
GeogramONE ggo;

uint8_t command5(uint8_t *, volatile uint8_t *opt1 = NULL, volatile uint8_t *opt2 = NULL, volatile uint8_t *opt3 = NULL);

volatile uint8_t call;
volatile uint8_t move;
volatile uint8_t battery = 0;
volatile uint8_t charge = 0;

uint8_t cmd0 = 0;
uint8_t cmd1 = 0;
uint8_t cmd3 = 0;
uint8_t cmd5 = 0;

uint8_t fence1 = 0;
uint8_t fence2 = 0;
uint8_t fence3 = 0;


uint32_t timeInterval = 0;


void setup()
{
	ggo.init();
	gps.init(115200);
	sim900.init(9600);
	max17043.init(7, 500);
	bma250.init(3, 500);
	max17043.initializeFuelGauge();
	attachInterrupt(0, ringIndicator, FALLING);
	attachInterrupt(1, movement, FALLING);
	PCintPort::attachInterrupt(PG_INT, &charger, CHANGE);
	PCintPort::attachInterrupt(FUELGAUGEPIN, &lowBattery, FALLING);
	sim900.goesWhere(smsData.smsNumber);
	call = sim900.checkForMessages();
	if(call == 0xFF)
		call = 0;
	battery = max17043.getAlertFlag();
	gps.isFenceActive(1, &fence1); 
	gps.isFenceActive(2, &fence2); 
	gps.isFenceActive(3, &fence3); 
}

void loop()
{
	gps.getTheData(&lastValid);
	if(call)
	{
		if(!sim900.getGeo(&smsData))
		{
			if(!smsData.smsPending)
				call = 0; // no more messages
			if(smsData.smsDataValid)
			{
				if(!smsData.smsCmdNum)
					cmd0 = 0x01;
				else if(smsData.smsCmdNum == 1)
					cmd1 = 0x01;
				else if(smsData.smsCmdNum == 2)
					command2();
				else if(smsData.smsCmdNum == 3)
					cmd3 = 0x01;
				else if(smsData.smsCmdNum == 4)
					command4();
				else if(smsData.smsCmdNum == 5)
					cmd5 = 0x01;
				else if(smsData.smsCmdNum == 6)
					command6();
			}
		}
	}
	if(cmd0)
		command0(&cmd0);
	if(cmd1)
		command1(&cmd1);
	if(cmd3)
		command3(&cmd3);
	if(cmd5)
		command5(&cmd5, &charge);
	if(timeInterval)
		timerMenu(&cmd0);
	if(battery)
	{
		if(!sim900.sendMessage(2,smsData.smsNumber,NULL,BATTERYMSG))
		{
			battery = 0;
			max17043.clearAlertFlag();
		}
	}
	if(charge & 0x01)
		chargerStatus();
	if(fence1)
	{
		static uint8_t breach1Conf = 0;
		static uint8_t previousSeconds1 = lastValid.seconds;
		if(fence1 == 1)
		{  
			if(!gps.configureFence(1,&fence))
				fence1 = 2;
		}
		if((fence1 == 2) && (lastValid.speedKnots >= BREACHSPEED))
		{  
			if(!gps.geoFenceDistance(&lastValid, &fence))
			{
				if(lastValid.seconds != previousSeconds1)
					breach1Conf++;
				if(breach1Conf > BREACHREPS)
				{
					fence1 = 3;
					breach1Conf = 0;
				}
				previousSeconds1 = lastValid.seconds;
			}
			else
				breach1Conf = 0;
		}
		else
			breach1Conf = 0;
		if(fence1 == 3)
		{  
			if(!sim900.sendMessage(2,smsData.smsNumber,NULL,FENCE1MSG))
				fence1 = 0;
		}
	} 
	if(fence2)
	{
		static uint8_t breach2Conf = 0;
		static uint8_t previousSeconds2 = lastValid.seconds;
		if(fence2 == 1)
		{  
			if(!gps.configureFence(2,&fence))
				fence2 = 2;
		}
		if((fence2 == 2) && (lastValid.speedKnots >= BREACHSPEED))
		{  
			if(!gps.geoFenceDistance(&lastValid, &fence))
			{
				if(lastValid.seconds != previousSeconds2)
					breach2Conf++;
				if(breach2Conf > BREACHREPS)
				{
					fence2 = 3;
					breach2Conf = 0;
				}
				previousSeconds2 = lastValid.seconds;
			}
			else
				breach2Conf = 0;
		}
		else
			breach2Conf = 0;
		if(fence2 == 3)
		{  
			if(!sim900.sendMessage(2,smsData.smsNumber,NULL,FENCE2MSG))
				fence2 = 0;
		}
	}	
	if(fence3)
	{
		static uint8_t breach3Conf = 0;
		static uint8_t previousSeconds3 = lastValid.seconds;
		if(fence3 == 1)
		{  
			if(!gps.configureFence(3,&fence))
				fence3 = 2;
		}
		if((fence3 == 2) && (lastValid.speedKnots >= BREACHSPEED))
		{  
			if(!gps.geoFenceDistance(&lastValid, &fence))
			{
				if(lastValid.seconds != previousSeconds3)
					breach3Conf++;
				if(breach3Conf > BREACHREPS)
				{
					fence3 = 3;
					breach3Conf = 0;
				}
				previousSeconds3 = lastValid.seconds;
			}
			else
				breach3Conf = 0;
		}
		else
			breach3Conf = 0;
		if(fence3 == 3)
		{  
			if(!sim900.sendMessage(2,smsData.smsNumber,NULL,FENCE3MSG))
				fence3 = 0;
		}
	}
} 

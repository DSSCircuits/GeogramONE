/*****************************************************************************
The Geogram ONE is an open source tracking device/development board based off 
the Arduino platform.  The hardware design and software files are released 
under CC-SA v3 license.
*****************************************************************************/

#include <AltSoftSerial.h>
#include <PinChangeInt.h>
#include "GeogramONE.h"
#include <EEPROM.h>
#include <I2C.h>
#include "eepromAnything.h"

#define USEFENCE1	1  //set to zero to free up code space if option is not needed
#define USEFENCE2	1  //set to zero to free up code space if option is not needed
#define USEFENCE3	1  //set to zero to free up code space if option is not needed
#define USESPEED	1  //set to zero to free up code space if option is not needed
#define USEMOTION	1  //set to zero to free up code space if option is not needed

GeogramONE ggo;
AltSoftSerial GSM;
SIM900 sim900(&GSM);
geoSmsData smsData;
PA6C gps(&Serial); 
goCoord lastValid;
geoFence fence;


volatile uint8_t call;
volatile uint8_t move;
volatile uint8_t battery = 0;
volatile uint8_t charge = 0x02; // force a read of the charger cable
volatile uint8_t d4Switch = 0x00;
volatile uint8_t d10Switch = 0x00;

uint8_t cmd0 = 0;
uint8_t cmd1 = 0;
uint8_t cmd3 = 0;
uint8_t udp = 0x00; 

//#if USEFENCE1
uint8_t fence1 = 0;
//#endif
//#if USEFENCE2
uint8_t fence2 = 0;
//#endif
//#if USEFENCE3
uint8_t fence3 = 0;
//#endif

uint8_t breachSpeed = 0;
uint8_t breachReps = 0;

uint32_t smsInterval = 0;
uint32_t udpInterval = 0;
uint32_t sleepTimeOn = 0;
uint32_t sleepTimeOff = 0;
uint8_t sleepTimeConfig = 0;

uint8_t speedHyst = 0;
uint16_t speedLimit = 0;

char udpReply[11];

void goesWhere(char *, uint8_t replyOrStored = 0);

void setup()
{
	ggo.init();
	gps.init(115200);
	sim900.init(9600);
	MAX17043init(7, 500);
	BMA250init(3, 500);
	attachInterrupt(0, ringIndicator, FALLING);
	attachInterrupt(1, movement, FALLING);
	PCintPort::attachInterrupt(PG_INT, &charger, CHANGE);
	PCintPort::attachInterrupt(FUELGAUGEPIN, &lowBattery, FALLING);
	goesWhere(smsData.smsNumber);
	call = sim900.checkForMessages();
	if(call == 0xFF)
		call = 0;
	battery = MAX17043getAlertFlag();
	#if USEFENCE1
	ggo.getFenceActive(1, &fence1);
	#endif
	#if USEFENCE2
	ggo.getFenceActive(2, &fence2);
	#endif
	#if USEFENCE3
	ggo.getFenceActive(3, &fence3);
	#endif
	#if USESPEED
	ggo.configureSpeed(&cmd3, &speedHyst, &speedLimit);
	#endif
	ggo.configureBreachParameters(&breachSpeed, &breachReps);
	ggo.configureInterval(&smsInterval, &sleepTimeOn, &sleepTimeOff, &sleepTimeConfig, &udpInterval);
	if(sleepTimeConfig & 0x02)
		BMA250enableInterrupts();
	uint8_t swInt = EEPROM.read(IOSTATE0);
	if(swInt == 0x05)
		PCintPort::attachInterrupt(4, &d4Interrupt, RISING);
	if(swInt == 0x06)
		PCintPort::attachInterrupt(4, &d4Interrupt, FALLING);
	swInt = EEPROM.read(IOSTATE1);
	if(swInt == 0x05)
		PCintPort::attachInterrupt(10, &d10Interrupt, RISING);
	if(swInt == 0x06)
		PCintPort::attachInterrupt(10, &d10Interrupt, FALLING);
}

void loop()
{
	if(!gps.getCoordinates(&lastValid))
	{
		int8_t tZ = EEPROM.read(TIMEZONE);
		bool eM = EEPROM.read(ENGMETRIC);
		gps.updateRegionalSettings(tZ, eM, &lastValid);
	}
	if(call)
	{
		sim900.gsmSleepMode(0);
		char pwd[5];
		EEPROM_readAnything(PINCODE,pwd);
		if(sim900.signalQuality())
		{
			if(!sim900.getGeo(&smsData, pwd))
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
						command5();
					else if(smsData.smsCmdNum == 6)
						command6();
					else if(smsData.smsCmdNum == 7)
						command7();
					else if(smsData.smsCmdNum == 8)
						command8();
					else if(smsData.smsCmdNum == 255)
					{
						sim900.gsmSleepMode(0);
						sim900.powerDownGSM();
						delay(2000);
						sim900.init(9600);
					}
				}
			}
		}
		sim900.gsmSleepMode(2);	
	}
	if(cmd0)
		command0();
	#if USEMOTION
	if(cmd1)
		command1();
	#endif
	#if USESPEED
	if(cmd3)
		command3();
	#endif
	if(udp)
		udpOrange();
	if(battery)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(BATTERYMSG);
			if(!sim900.sendSMS())
			{
				battery = 0;
				MAX17043clearAlertFlag();
			}
		}
		sim900.gsmSleepMode(2);
	}
	if(charge & 0x02)
		chargerStatus();
	#if USEFENCE1
	if(fence1)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach1Conf = 0;
		static char previousSeconds1 = lastValid.time[5];
		if((fence1 == 1) && (lastValid.speed >= breachSpeed))
		{
			ggo.configureFence(1,&fence); 
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds1)
					breach1Conf++;
				if(breach1Conf > breachReps)
				{
					fence1 = 2;
					breach1Conf = 0;
				}
				previousSeconds1 = lastValid.time[5];
			}
			else
				breach1Conf = 0;
		}
		else
			breach1Conf = 0;
		if(fence1 == 2)
		{
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE1MSG);
				if(!sim900.sendSMS())
					fence1 = 0x00;
			}
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	#if USEFENCE2
	if(fence2)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach2Conf = 0;
		static char previousSeconds2 = lastValid.time[5];
		if((fence2 == 1) && (lastValid.speed >= breachSpeed))
		{  
			ggo.configureFence(2,&fence);
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds2)
					breach2Conf++;
				if(breach2Conf > breachReps)
				{
					fence2 = 2;
					breach2Conf = 0;
				}
				previousSeconds2 = lastValid.time[5];
			}
			else
				breach2Conf = 0;
		}
		else
			breach2Conf = 0;
		if(fence2 == 2)
		{  
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE2MSG);
				if(!sim900.sendSMS())
					fence2 = 0x00;
			}
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	#if USEFENCE3
	if(fence3)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach3Conf = 0;
		static char previousSeconds3 = lastValid.time[5];
		if((fence3 == 1) && (lastValid.speed >= breachSpeed))
		{  
			ggo.configureFence(3,&fence);
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds3)
					breach3Conf++;
				if(breach3Conf > breachReps)
				{
					fence3 = 2;
					breach3Conf = 0;
				}
				previousSeconds3 = lastValid.time[5];
			}
			else
				breach3Conf = 0;
		}
		else
			breach3Conf = 0;
		if(fence3 == 2)
		{  
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE3MSG);
				if(!sim900.sendSMS())
					fence3 = 0x00;
			}	
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	if(smsInterval)
		smsTimerMenu();
	if(udpInterval)
		udpTimerMenu();
	if(sleepTimeOn && sleepTimeOff)
		sleepTimer();
	if(d4Switch)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(D4MSG);
			if(!sim900.sendSMS())
				d4Switch = 0x00;
		}
		sim900.gsmSleepMode(2);
	}
	if(d10Switch)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(D10MSG);
			if(!sim900.sendSMS())
				d10Switch = 0x00;
		}
		sim900.gsmSleepMode(2);
	}
} 

void printEEPROM(uint16_t eAddress)
{
	char eepChar;
	for (uint8_t ep = 0; ep < 50; ep++)
	{
		eepChar = EEPROM.read(ep + eAddress);
		if(eepChar == '\0')
			break;
		else
			GSM.print(eepChar);
	}
}

void goesWhere(char *smsAddress, uint8_t replyOrStored)
{
	if(!replyOrStored)
		EEPROM_readAnything(RETURNADDCONFIG,replyOrStored);
	if((replyOrStored == 2) || ((replyOrStored == 3) && (smsAddress[0] == NULL)))
	for(uint8_t l = 0; l < 39; l++)
	{
			smsAddress[l] = EEPROM.read(l + SMSADDRESS);
			if(smsAddress[l] == NULL)
				break;
	}
}

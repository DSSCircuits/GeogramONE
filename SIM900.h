#include <AltSoftSerial.h>
//#include <avr/pgmspace.h>
#include <EEPROM.h>

#ifndef SIM900_h
#define SIM900_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define TIMESTORETRY    3
#define GSMSTATUS       5      
#define GSMSWITCH       6
#define PINCODE         0
#define SIMSIZE			20

#define IPR				"AT+IPR=9600"  	//basic
#define CMGR			"AT+CMGR=" 		//ext
#define CMGD			"AT+CMGD="		//basic
#define CPMS			"AT+CPMS?"		//ext
#define CMGF			"AT+CMGF=1"	//basic
#define CNMI			"AT+CNMI=0,0,0,0,0"	//basic
#define CREG			"AT+CREG?"		//ext
#define CSCLK			"AT+CSCLK="		//basic
#define CPOWD			"AT+CPOWD=1"	//
#define CMGDA			"AT+CMGDA=\"DEL ALL\""	//basic
#define CMEE			"AT+CMEE=1" 	//basic
#define CSQ				"AT+CSQ" 		//ext

#define	OK				"OK"
#define	ERROR			"ERROR"
#define CALLREADY		"Call Ready\r\n"


#define AT_TO			200L    	//1000
#define CMGR_TO			15000L	//15000
#define CMGD_TO			15000L	//15000
#define CMGDA_TO		10000L	//10000
#define CSQ_TO			30000L	//30000
#define CSCLK_TO		2000L   	//2000
#define CPOWD_TO		10000L  	//10000
#define CMEE_TO			10000L  	//10000
#define IPR_TO			10000L  	//10000
#define CMGF_TO			10000L  	//10000
#define CNMI_TO			10000L  	//10000
#define CREG_TO			30000L  	//30000
#define CPMS_TO			10000L		//10000
#define CPIN_TO			10000L
#define CMGS1_TO		2000L
#define CMGS2_TO		20000L

#define DEBUGON			1

/*******EEPROM ADDRESSES**********/
#define PINCODE					0
#define SMSADDRESS				5
#define EMAILADDRESS			44
#define APN						83
#define RETURNADDCONFIG			87

struct simSmsData
{
    char smsNumber[39];
    char smsDate[11];
    char smsTime[13];
	char smsPwd[5];
	uint8_t smsCmdNum;
	char smsCmdString[30];
};

struct geoSmsData
{
	bool smsDataValid;
    char smsNumber[39];
	uint8_t smsCmdNum;
	char smsCmdString[30];
	uint8_t smsPending;
};

class SIM900 
{
	public:
		SIM900(AltSoftSerial *ser);
		int freeRam();
		uint8_t init(unsigned long);
		uint8_t checkForMessages();
		bool deleteMessage(int);
		uint8_t readMessageBreakOut(simSmsData *, int);
		uint8_t sendMessage(uint8_t, char *, const char *, uint16_t eepromMsgAddress = 1024);
		uint8_t goesWhere(char *);
		uint8_t confirmPassword(char *, char *);
		bool deleteAllMessages();
		uint8_t getCommand(char *);
		bool gsmSleepMode0();
		bool gsmSleepMode2();
		uint8_t signalQuality(bool wakeUpComm = false);
		uint8_t powerDownGSM();
		uint8_t getGeo(geoSmsData *);
		void printLatLon(long *, long *);
	private:
		bool atNoData(const char *, unsigned long, int argument = 0xFF);
		uint8_t totalMsg;
		uint8_t currentMsg;
		uint8_t powerOnGSM();
		void initializeGSM();
		bool checkNetworkRegistration();
		bool sendATCommandBasic(char*, uint8_t, unsigned long);
		bool callReady();
		AltSoftSerial *GSM;
};

#endif
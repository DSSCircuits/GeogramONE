#include <AltSoftSerial.h>
#include <avr/pgmspace.h>
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

#define AT_TO			200L    	//1000
#define CMGR_TO			300L	//15000
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

#define INDEX_SIZE		100
#define DELIMITER		"."
#define OK				"OK"

/*******EEPROM ADDRESSES**********/
#define PINCODE					0
#define SMSADDRESS				5
#define EMAILADDRESS			44
#define APN						83
#define RETURNADDCONFIG			87

struct simSmsData
{
    char smsNumber[39];
//    char smsDate[11];
//    char smsTime[13];
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
		char atRxBuffer[INDEX_SIZE];
		uint8_t init(unsigned long);
		uint8_t checkForMessages();
		uint8_t deleteMessage(int);
		uint8_t readMessageBreakOut(simSmsData *, int);
		uint8_t sendMessage(uint8_t, char *, const char *, uint16_t eepromMsgAddress = 1024);
		uint8_t goesWhere(char *);
		uint8_t confirmPassword(char *, char *);
		uint8_t deleteAllMessages();
		uint8_t getCommand(char *);
		uint8_t gsmSleepMode(int);
		uint8_t signalQuality();
		uint8_t powerDownGSM();
		uint8_t getGeo(geoSmsData *);
		void printLatLon(long *, long *);
		uint8_t confirmAtCommand(char *, unsigned long);
	private:
		void printPROGMEM(int, int atArg = 0xFF);
		uint8_t totalMsg;
		uint8_t currentMsg;
		uint8_t powerOnGSM();
		void initializeGSM();
		uint8_t checkNetworkRegistration();
		uint8_t callReady();
		AltSoftSerial *GSM;
};

#endif
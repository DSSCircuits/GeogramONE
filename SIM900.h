#include <AltSoftSerial.h>

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
#define UDPREPLY_TO		5000L

#define INDEX_SIZE		100
#define DELIMITER		"."
#define OK				"OK"

#define BAUD_RATE		9600UL

/*******EEPROM ADDRESSES**********/

struct geoSmsData
{
	bool smsDataValid;
    char smsNumber[39];
	uint32_t emailAPN;
	uint8_t smsCmdNum;
	char smsCmdString[70];
	uint8_t smsPending;
};

class SIM900 
{
	public:
		SIM900(AltSoftSerial *ser);
		char atRxBuffer[INDEX_SIZE];
		uint8_t init(unsigned long baudRate = BAUD_RATE);
		uint8_t checkForMessages();
		uint8_t deleteMessage(int);
		uint8_t readMessageBreakOut(int);
		bool prepareSMS(char *, uint32_t);
		uint8_t sendSMS();
		uint8_t deleteAllMessages();
		uint8_t gsmSleepMode(int);
		uint8_t signalQuality();
		uint8_t powerDownGSM();
		void rebootGSM();
		void initializeGSM();
		bool checkNetworkRegistration();
		uint8_t getGeo(geoSmsData *, char *);
		uint8_t confirmAtCommand(char *, unsigned long);
		uint8_t cipStatus();
		void getIMEI(char *);
	private:
		uint8_t powerOnGSM();
		uint8_t callReady();
		AltSoftSerial *GSM;
};

#endif
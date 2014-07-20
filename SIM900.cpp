#include "SIM900.h"

SIM900::SIM900(AltSoftSerial *ser)
{
	GSM = ser;
}

uint8_t SIM900::init(unsigned long baudRate)
{
	pinMode(2,INPUT);
	pinMode(GSMSTATUS,INPUT);
	pinMode(GSMSWITCH,OUTPUT);
	digitalWrite(GSMSWITCH,LOW);
	GSM->begin(BAUD_RATE);
	initializeGSM();
//	gsmSleepMode(2);
}

/*************************************************************	
Procedure to put GSM module into sleep mode.  Two sleep modes
are available. Mode 0 - GSM taken out of sleep mode.  Mode 2 - 
GSM enters sleep mode if UART is idle for 5 or more seconds

ARGUMENTS
	mode			0 or 2
	
RETURN:
	0				command successful
	1				ERROR executing command
	2				Buffer full, potential problem
	3				Timeout reached, potential problem
**************************************************************/
uint8_t SIM900::gsmSleepMode(int mode)
{
	GSM->println("AT\r\nAT");
	delay(50);
	GSM->print("AT+CSCLK=");
	GSM->println(mode);
	return(confirmAtCommand("OK",CSCLK_TO));
}


/*************************************************************	
Procedure to search data returned from AT commands.  All 
data up to and including searchString is stored in a global
array to be used if needed.

ARGUMENTS
	searchString	pointer to string to be searched
	timeOut			timeout in milliseconds
	
RETURN:
	0				searchString found successfully
	1				ERROR string encountered, potential problem
	2				Buffer full, searchString not found
	3				Timeout reached before searchString found
**************************************************************/
uint8_t SIM900::confirmAtCommand(char *searchString, unsigned long timeOut)
{
	uint8_t index = 0;
	unsigned long tOut = millis();
	while((millis() - tOut) <= timeOut)
	{
		if (GSM->available())
		{
			atRxBuffer[index] = GSM->read();
			index++;
			atRxBuffer[index] = '\0';
			if(strstr(atRxBuffer,searchString) != NULL)
				return 0;
			if(strstr(atRxBuffer,"ERROR") != NULL)
				return 1;
			if(index >= (INDEX_SIZE - 1))
				return 2;	
		}
	}
	return 3;
}

/*************************************************************	
Procedure to power on the GSM module.  It will first check 
if the module is already turned on.
RETURN:
	0		GSM turned on successfully or was already on
	1		GSM module did not turn on
	2		GSM was already powered on
**************************************************************/
uint8_t SIM900::powerOnGSM(){
	if(digitalRead(GSMSTATUS))
		return 2; //GSM is already powered on
	for(uint8_t i = 0; i < TIMESTORETRY;i++) //was one before
	{
		digitalWrite(GSMSWITCH,HIGH); //send signal to turn on
		delay(1100);  //signal needs to be low for 1 second to turn GPRS on
		digitalWrite(GSMSWITCH,LOW);
		delay(2300);
		if(digitalRead(GSMSTATUS))
			return 0; //GSM powered on successfully
	}
	return 1; //the GSM did not turn on
}


/*************************************************************	
Procedure to power off the GSM module.  Power off is
done through software commands, not through hardware
RETURN:
	0		GSM turned off successfully
	1		GSM module did not turn off
**************************************************************/
uint8_t SIM900::powerDownGSM()
{
	GSM->println("AT+CPOWD=1");
	confirmAtCommand("DOWN",CPOWD_TO);
	if(!digitalRead(GSMSTATUS))
		return 1; //GSM did not power off successfully
	return 0;
}

void SIM900::rebootGSM()
{
	gsmSleepMode(0);
	powerDownGSM();
	delay(2000);
	initializeGSM();
}


void SIM900::initializeGSM()
{
	uint8_t powerOK = powerOnGSM();
	if(powerOK == 2) //GSM was powered on already
		return;
	if(!powerOK)
	{
		callReady();
		GSM->println("AT+CMEE=1");
		confirmAtCommand("OK",CMEE_TO);
		GSM->print("AT+IPR=");
		GSM->println(BAUD_RATE,DEC);
		confirmAtCommand("OK",IPR_TO);
		GSM->println("AT+CMGF=1");
		confirmAtCommand("OK",CMGF_TO);
		GSM->println("AT+CNMI=0,0,0,0,0");
		confirmAtCommand("OK",CNMI_TO);
		gsmSleepMode(2);
	}
}

/*************************************************************	
Procedure to check if GSM is registered to the network. 
RETURN:
	0		GSM is registered to the network
	1		GSM is not registered to the network
**************************************************************/
bool SIM900::checkNetworkRegistration()
{
	GSM->println("AT+CREG?");
	confirmAtCommand("OK",CREG_TO);
	if((strstr(atRxBuffer,",1") != NULL) || (strstr(atRxBuffer,",5") != NULL))
		return 0;
	return 1;
}

/*************************************************************	
Procedure to check for messages on the SIM card 
RETURN:
	x		x = number of messages on the SIM card
	0xFF	No messages on the SIM card
**************************************************************/
uint8_t SIM900::checkForMessages()
{
	GSM->println("AT+CPMS?");
	if(confirmAtCommand("OK",CPMS_TO))
		return 0xFF;
	char *ptr = atRxBuffer;
	char *str = NULL;	
	ptr = strtok_r(ptr,",",&str);
	ptr = strtok_r(NULL,",",&str);
	return (atoi(ptr));  //Number of messages on the sim card
}


/*************************************************************	
Procedure to delete message from SIM card.
ARGUMENT:
	msg				message number to delete
 
RETURN:
	0				command successful
	1				ERROR executing command
	2				Buffer full, potential problem
	3				Timeout reached, potential problem
**************************************************************/
uint8_t SIM900::deleteMessage(int msg)
{
	GSM->print("AT+CMGD=");
	GSM->println(msg);
	return(confirmAtCommand("OK",CMGD_TO));
}

/*************************************************************	
Procedure to delete all messages from SIM card.
 
RETURN:
	0				command successful
	1				ERROR executing command
	2				Buffer full, potential problem
	3				Timeout reached, potential problem
**************************************************************/
uint8_t SIM900::deleteAllMessages()
{
	GSM->println("AT+CMGDA=\"DEL ALL\"");
	return(confirmAtCommand(OK,CMGD_TO));
}

/*************************************************************	
Procedure to prepare SMS. Sending and SMS can be broken down 
into 3 parts.  Prepare the SMS header, print data and send data.

ARGUMENT:
	smsAddress		pointer to SMS address
 
RETURN:
	0				command successful
	1				ERROR executing command, SMS aborted
**************************************************************/
bool SIM900::prepareSMS(char *smsAddress, uint32_t apn)
{
	bool isEmail = false;
	if(strlen(smsAddress) < 3)
		return true;  //address field is blank
	if(strstr(smsAddress,"@") != NULL)
		isEmail = true;
	GSM->print("AT+CMGS=\"");
	if(isEmail)
		GSM->print(apn);
	else
		GSM->print(smsAddress);
	GSM->println("\"");
	if(!confirmAtCommand(">",CMGS1_TO))
	{
		if(isEmail)
			GSM->println(smsAddress);
		return false;
	}
	GSM->println((char)0x1B); //do not send message
	delay(500);
	return true;  //There was an error waiting for the > 
}

/*************************************************************	
Send the SMS. Sending and SMS can be broken down 
into 3 parts.  Prepare the SMS header, print data and send data.

ARGUMENT:
	smsAddress		pointer to SMS address
 
RETURN:
	0				SMS successfully sent
	1				ERROR executing command
	2				Buffer full, potential problem
	3				Timeout reached, potential problem
**************************************************************/	
uint8_t SIM900::sendSMS()
{
	GSM->println((char)0x1A);
    unsigned long timeOut = millis();
	while ((millis() - timeOut) <= CMGS2_TO)
	{
		if (GSM->available())
		{
			if(GSM->read() == 0x1A)
				break;
		}
	}
	return(confirmAtCommand("OK",CMGS2_TO));
}




uint8_t SIM900::readMessageBreakOut(int msg)
{
	GSM->print("AT+CMGR=");
	GSM->println(msg);
	if(confirmAtCommand("D\",\"",CMGR_TO))
	{
		if(strlen(atRxBuffer) <= 20)
			return 0xFF;
		return 1;
	}
	return 0;
}

uint8_t SIM900::getGeo(geoSmsData *retSms, char *pwd)
{
	static uint8_t l = 1;
	retSms->smsDataValid = false;
	retSms->smsCmdNum = 0xFF;
	retSms->smsPending = checkForMessages();
	if(!retSms->smsPending)
	{
		l = 1;
		return 0;//no messages
	}
	if(retSms->smsPending == 0xFF)
	{
		retSms->smsPending = 0x00;
		return 1; //there was an error
	} 
	while(retSms->smsPending && (l <= SIMSIZE))
	{
		if(readMessageBreakOut(l) == 0xFF)
		{
			l++;
			continue;
		}
		confirmAtCommand("\"",100);
		atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
		strcpy(retSms->smsNumber,atRxBuffer);
		confirmAtCommand("\r\n",100);
		if(!strstr(retSms->smsNumber,"+") != NULL)
		{
			confirmAtCommand("/",500);
			confirmAtCommand("/",500);
		}
		confirmAtCommand(DELIMITER,100);
		atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
		if(strncmp((atRxBuffer + (strlen(atRxBuffer)-4)),pwd,4) != 0) 
			retSms->smsNumber[0] = '\0';
		else
		{
			retSms->smsDataValid = true;
			confirmAtCommand(DELIMITER,100);
			atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
			retSms->smsCmdNum = atoi(atRxBuffer);
			confirmAtCommand("\r\n",100);
			strcpy(retSms->smsCmdString,atRxBuffer);
		}
		deleteMessage(l);
		retSms->smsPending--;
		l++;
		if(!retSms->smsPending)
			l = 1;
		return 0;  //message read
	}
	deleteAllMessages(); // too many messages on SIM card
	return 2;
}

	


/*************************************************************	
	Procedure to check if the GSM module is ready to make and
	receive calls
	RETURN:
		0		GSM is ready
		1		Error, buffer is full
		1		GSM timed out, not ready
**************************************************************/
uint8_t SIM900::callReady()
{
	return(confirmAtCommand("l Ready",30000));
}


/*************************************************************	
	Procedure to check the GSM signal quality. The higher the
	number the better the signal quality.
	RETURN:
		0			Error sending AT command/No signal
		1 - 99		RSSI signal strength
		
**************************************************************/
uint8_t SIM900::signalQuality()
{
	GSM->println("AT+CSQ");
	if(confirmAtCommand("OK",CSQ_TO))
		return 0;
	char *ptr = atRxBuffer;
	char *str = NULL;	
	ptr = strtok_r(ptr," ",&str);
	ptr = strtok_r(NULL,",",&str);
	return(atoi(ptr));
}


uint8_t SIM900::cipStatus()
{
	GSM->println("AT+CIPSTATUS");
	if(confirmAtCommand("TE:",1000))
		return 0xFF; //problem with command
	confirmAtCommand("\n",500);
	if(strstr(atRxBuffer,"IP I") != NULL)
		return 0;
	if(strstr(atRxBuffer,"ART") != NULL)
		return 1;
	if(strstr(atRxBuffer,"IP C") != NULL)
		return 2;
	if(strstr(atRxBuffer,"IP G") != NULL)
		return 3;
	if(strstr(atRxBuffer,"TUS") != NULL)
		return 4;
	if(strstr(atRxBuffer,"G/S") != NULL)
		return 5;
	if(strstr(atRxBuffer,"T OK") != NULL)
		return 6;	
	if(strstr(atRxBuffer,"SIN") != NULL)
		return 7;	
	if(strstr(atRxBuffer,"SED") != NULL)
		return 8;		
	if(strstr(atRxBuffer,"PDP") != NULL)
		return 9;
	return 0xFF;
}

void SIM900::getIMEI(char *imei)
{
	GSM->println("AT+GSN");
	if(confirmAtCommand("OK",2000))
		return;
	char *ptr = atRxBuffer;
	char *str = NULL;	
	ptr = strtok_r(ptr,"\r\n",&str);
	ptr = strtok_r(NULL,"\r\n",&str);
	for(uint8_t e = 0; e < 16; e++)
	{
		imei[e] = ptr[e];
	}
	imei[15] = '\0';
	return;
}
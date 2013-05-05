#include "SIM900.h"


template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
    return i;
}

//const char* DELIMITER = ".";

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
	GSM->begin(baudRate);
	totalMsg = 0;
	initializeGSM();
	gsmSleepMode(2);
}

uint8_t SIM900::gsmSleepMode(int mode)
{
	GSM->println("AT\r\nAT");
	delay(50);
	GSM->print("AT+CSCLK=");
	GSM->println(mode);
	return(confirmAtCommand("OK",CSCLK_TO));
}


uint8_t SIM900::confirmAtCommand(char *returnCode, unsigned long atTimeOut)
{
	uint8_t index = 0;
	unsigned long timeOut = millis();
	while((millis() - timeOut) <= atTimeOut)
	{
		if (GSM->available())
		{
			atRxBuffer[index] = GSM->read();
			index++;
			atRxBuffer[index] = '\0';
			if(strstr(atRxBuffer,returnCode) != NULL)
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


void SIM900::initializeGSM()
{
	if(!powerOnGSM())
		callReady();
	GSM->println("AT+CMEE=1");
	confirmAtCommand("OK",CMEE_TO);
	GSM->println("AT+IPR=9600");
	confirmAtCommand("OK",IPR_TO);
	GSM->println("AT+CMGF=1");
	confirmAtCommand("OK",CMGF_TO);
	GSM->println("AT+CNMI=0,0,0,0,0");
	confirmAtCommand("OK",CNMI_TO);
}


uint8_t SIM900::checkNetworkRegistration()
{
	GSM->println("AT+CREG?");
	confirmAtCommand("OK",CREG_TO);
	if(strstr(atRxBuffer,",1") != NULL || strstr(atRxBuffer,",5") != NULL)
		return 0; //GSM is registered on the network
	return 1; //GSM is not registered on the network
}

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

uint8_t SIM900::deleteMessage(int msg)
{
	GSM->print("AT+CMGD=");
	GSM->println(msg);
	return(confirmAtCommand("OK",CMGD_TO));
}

uint8_t SIM900::deleteAllMessages()
{
	GSM->println("CMGDA=\"DEL ALL\"");
	return(confirmAtCommand(OK,CMGD_TO));
}

uint8_t SIM900::readMessageBreakOut(simSmsData *sms, int msg)
{
	GSM->print("AT+CMGR=");
	GSM->println(msg);
	if(confirmAtCommand("D\",\"",CMGR_TO))
	{
		if(strlen(atRxBuffer) <= 20)
			return 0xFF;
		return 1;
	}
	confirmAtCommand("\"",100);
	atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
	strcpy(sms->smsNumber,atRxBuffer);
	confirmAtCommand("\r\n",500);
	confirmAtCommand(DELIMITER,500);
	atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
	strcpy(sms->smsPwd,atRxBuffer + (strlen(atRxBuffer)-4));
	confirmAtCommand(DELIMITER,500);
	atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
	sms->smsCmdNum = atoi(atRxBuffer);
	confirmAtCommand(DELIMITER,500);
	atRxBuffer[strlen(atRxBuffer) - 1] = '\0';
	if(strlen(atRxBuffer) > 29)
		atRxBuffer[29] = '\0';
	strcpy(sms->smsCmdString,atRxBuffer);
	return 0;
}

uint8_t SIM900::goesWhere(char *smsAddress)
{
	uint8_t replyOrStored = 0;
	EEPROM_readAnything(RETURNADDCONFIG,replyOrStored);
	for(uint8_t l = 0; l < 39; l++)
	{
		if((replyOrStored == 1) || (replyOrStored == 3))
			smsAddress[l] = EEPROM.read(l + SMSADDRESS);
		else if(replyOrStored == 2)
			smsAddress[l] = EEPROM.read(l + EMAILADDRESS);
		if(smsAddress[l] == '\0')
			break;
	}
}



/*************************************************************	
	Procedure to send an SMS message.  Three options are 
	available to send the message.  Option 1 sends the 
	header information only, option 2 sends the complete
	message using the message passed as an argument. 
    The third option only sends the footer.
	
	ARGUMENTS:
		uint8_t smsFormat	
				0	send header only
				1	send header, passed message and footer
				2	send header, message at eeprom address and footer
				3	send footer only
				
				
		char *smsAddress	
				phone number or email address
				
		const char *smsMessage
				message to be sent
				
		uint16_t eepromMsgAddress
				address of eeprom message
				
	
	RETURN:
		0		SMS portion successfully issued
		1		Time out waiting for > , message not sent
		2		Error sending SMS
		3		Time out waiting for acknowledge of sent SMS
		4		No GSM signal
**************************************************************/
uint8_t SIM900::sendMessage(uint8_t smsFormat, char *smsAddress, const char *smsMessage, uint16_t eepromMsgAddress)
{
	unsigned long timeOut = 0;
	boolean ns = false;
	if(smsFormat < 3)  //might want to put a sleepmode0 in here
	{
		if(!signalQuality())
			return 4;
		GSM->print("AT+CMGS=\"");
		GSM->print(smsAddress);
		GSM->println("\"");	
		if(!confirmAtCommand(">",CMGS1_TO))
			ns = true;
		if(!ns)
		{
			GSM->println((char)0x1B); //do not send message
			delay(500);
			GSM->flush();
			return 1;  //There was an error waiting for the > 
		} 
		if(!smsFormat)
			return 0; //Header information successfully sent
		if(smsFormat == 1)
			GSM->println(smsMessage);
		if(smsFormat == 2)
		{
			char eepChar;
			for (uint8_t ep = 0; ep < 50; ep++)
			{
				eepChar = EEPROM.read(ep + eepromMsgAddress);
				if(eepChar == '\0')
					break;
				else
					GSM->print(eepChar);
			}
			GSM->println();
		}
	}
    GSM->println((char)0x1A);
    timeOut = millis();
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


uint8_t SIM900::confirmPassword(char *smsPwd, char *pwd)
{
	if(strncmp(smsPwd,pwd,4) == 0) //debug serial print
		return 0; //valid security code
	return 1;
}

uint8_t SIM900::getGeo(geoSmsData *retSms)
{
	static uint8_t l = 1;
	simSmsData sms;
	retSms->smsDataValid = false;
	retSms->smsCmdNum = 0xFF;
	if(!signalQuality())
		return 3;
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
		if(readMessageBreakOut(&sms, l) == 0xFF)
		{
			l++;
			continue;
		}
		deleteMessage(l);
		retSms->smsPending--;
		l++;
		retSms->smsCmdNum = sms.smsCmdNum;
		strcpy(retSms->smsCmdString,sms.smsCmdString);
		strcpy(retSms->smsNumber,sms.smsNumber);
		char pwd[5];
		EEPROM_readAnything(PINCODE,pwd);
		if(!confirmPassword(sms.smsPwd,pwd))
			retSms->smsDataValid = true;
		else
			retSms->smsDataValid = false;
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
	return(confirmAtCommand("Call Ready",30000));
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

void SIM900::printLatLon(long *lat, long *lon)
{
		if(*lat < 0)
			GSM->print("-");
        GSM->print(abs(*lat/1000000));
        GSM->print("+");
        GSM->print(abs((*lat%1000000)/10000.0),4);
        GSM->print(",");
		if(*lon < 0)
			GSM->print("-");
        GSM->print(abs(*lon/1000000));
        GSM->print("+");
        GSM->print(abs((*lon%1000000)/10000.0),4);
}

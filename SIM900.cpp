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


const char* DELIMITER = ".";

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
	gsmSleepMode2();
}

bool SIM900::gsmSleepMode0()
{
	GSM->println("AT\r\nAT");
	delay(50);
	return(atNoData(CSCLK,CSCLK_TO,0));
}

bool SIM900::gsmSleepMode2()
{
	GSM->println("AT\r\nAT");
	return(atNoData(CSCLK,CSCLK_TO,2));
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
	char rxBuffer[50];
	gsmSleepMode2();
	sendATCommandBasic(CPOWD,rxBuffer,50,CPOWD_TO,0,false);
	if(!digitalRead(GSMSTATUS))
		return 1; //GSM did not power off successfully
	return 0;
}


void SIM900::initializeGSM()
{
	if(!powerOnGSM())
		callReady();
	atNoData(CMEE,CMEE_TO); //CMEE
	atNoData(IPR,IPR_TO); //IPR
	atNoData(CMGF,CMGF_TO); //CMGF
	atNoData(CNMI,CNMI_TO); //CNMI
}

bool SIM900::atNoData(const char *atCommand, unsigned long atTimeOut, int argument)
{
	uint8_t index = 0;
	char rxBuffer[22];
	GSM->flush();
	GSM->print("AT+");
	GSM->print(atCommand);
	if(argument == 0xFF)
		GSM->println();
	else
		GSM->println(argument);
	unsigned long timeOut = millis();
	while((millis() - timeOut) <= atTimeOut)
	{
		if (GSM->available())
		{
			rxBuffer[index] = GSM->read();
			index++;
			rxBuffer[index] = '\0';
			if(strstr(rxBuffer,OK) != NULL)
				return 0;
			if(strstr(rxBuffer,ERROR) != NULL)
				return 1;
			if(index == 21)
				index = 0;
		}
	}
	return 1; //timed out
}

bool SIM900::checkNetworkRegistration()
{
	char rxBuffer[50];
	sendATCommandBasic(CREG,rxBuffer,50,CREG_TO,0,false);  //check network registration status
	if((strstr(rxBuffer,",1") != NULL) || (strstr(rxBuffer,",5") != NULL))
		return 0; //GSM is registered on the network
	return 1; //GSM is not registered on the network
}

uint8_t SIM900::checkForMessages()
{
	char rxBuffer[75]; //was 100
	if(sendATCommandBasic(CPMS,rxBuffer,75,CPMS_TO,0,false)){return 0xFF;}  //check for new messages
	char *ptr = rxBuffer;
	char *str = NULL;
	ptr = strtok_r(ptr,",",&str);
	ptr = strtok_r(NULL,",",&str);
	return (atoi(ptr));  //Number of messages on the sim card
}

bool SIM900::deleteMessage(uint8_t msg)
{
	return(atNoData(CMGD,CMGD_TO,msg));
}

bool SIM900::deleteAllMessages()
{
	return(atNoData(CMGDA,CMGDA_TO));
}

uint8_t SIM900::readMessageBreakOut(simSmsData *sms, uint8_t msg)
{
	char rxBuffer[100];
	char* str = NULL;
	char* ptr = NULL;
	uint8_t replyMessageType;
	if(sendATCommandBasic(CMGR,rxBuffer,100,CMGR_TO,msg,true)){return 1;}
	if(strlen(rxBuffer) <= 15){return 0xFF;} //if string length is under 15, SIM position was blank
	if(strstr(rxBuffer,"@")!=NULL)
		replyMessageType = 2; //received message type is an email
	else
		replyMessageType = 1; //received message type is an SMS
	ptr = strtok_r(rxBuffer,":",&str); //pull phone number from string
	if(replyMessageType == 1) //message is an SMS
	{
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		if (strlen(ptr) < 20)  //if phone number is 20 digits or less then it's OK to use
			strcpy(sms->smsNumber,ptr);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,",",&str);
		strcpy(sms->smsDate,ptr);
		ptr = strtok_r(NULL,"\"",&str);
		strcpy(sms->smsTime,ptr);
		sms->smsTime[8] = '\0';
		ptr = strtok_r(NULL,"\n",&str);
	}	
	else if(replyMessageType == 2) //message is an email
	{
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,",",&str);
		strcpy(sms->smsDate,ptr);
		ptr = strtok_r(NULL,"\"",&str);
		strcpy(sms->smsTime,ptr);
		sms->smsTime[8] = '\0';
		ptr = strtok_r(NULL,"\n",&str); 
		ptr = strtok_r(NULL,"/",&str);
		if (strlen(ptr) < 39)  //if email address is 39 digits or less then it's OK to use
			strcpy(sms->smsNumber,ptr);
		ptr = strtok_r(NULL,"/",&str);
	}
	ptr = strtok_r(NULL,"\n",&str);
	ptr = strtok_r(ptr,DELIMITER,&str);
	ptr += (strlen(ptr)-4);
	strcpy(sms->smsPwd,ptr);
	ptr = strtok_r(NULL,DELIMITER,&str);
	sms->smsCmdNum = atoi(ptr);
	if(strlen(str) > 29)
		str[29] = '\0';
	strcpy(sms->smsCmdString,str);
	return 0;
}

uint8_t SIM900::goesWhere(char *smsAddress)
{
	uint8_t replyOrStored = 0;
	EEPROM_readAnything(RETURNADDCONFIG,replyOrStored);
	for(uint8_t l = 0; l < 39; l++)
	{
		if(replyOrStored == 1)
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
	char rxBuffer[20];
	unsigned long timeOut = 0;
	boolean ns = false;
	if(smsFormat < 3)  //might want to put a sleepmode0 in here
	{
		if(!signalQuality(1))
			return 4;
		uint8_t addressType = 0;
		goesWhere(smsAddress);
		if(strstr(smsAddress,"@") != NULL){addressType = 1;} //address is an email
		GSM->print("AT+CMGS=\"");
		switch(addressType)
		{
			case 0:
				GSM->print(smsAddress);
				break;
			case 1:
				{
					unsigned long apn;
					EEPROM_readAnything(APN,apn);
					GSM->print(apn,DEC);
				}
				break;
			GSM->println("\"");
		}
		timeOut = millis();
		while ((millis() - timeOut) <= CMGS1_TO)
		{
			if(GSM->available())
			{
				if(GSM->read() == '>')
				{
					ns = true;
					break;
				}
			}
		}
		if(!ns)
		{
			GSM->println((char)0x1B); //do not send message
			delay(500);
			GSM->flush();
			return 1;  //There was an error waiting for the > 
		} 
		if(addressType == 1)
			GSM->println(smsAddress);
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
	uint8_t idx = 0;
    while ((millis() - timeOut) <= CMGS2_TO)
    {
		if (GSM->available())
		{
			rxBuffer[idx] = GSM->read();
			idx++;
			rxBuffer[idx] = '\0';
			if(strstr(rxBuffer,ERROR)!= NULL)
			{
				return 2;  // There was an error sending the SMS/email
			}
			if(strstr(rxBuffer,OK)!= NULL)
			{
				return 0;  // SMS/email successfully sent
			}
			if((rxBuffer[idx-1] == '\n') || (idx == 19) )
				idx = 0;
		}
	}
	return 3;  //There was a timeout error
}


uint8_t SIM900::confirmPassword(char *smsPwd, char *pwd)
{
	if(strncmp(smsPwd,pwd,4) == 0) //debug serial print
	{
		return 0; //valid security code
	}
	return 1;
}

uint8_t SIM900::getGeo(geoSmsData *retSms)
{
	simSmsData sms;
	retSms->smsDataValid = false;
	retSms->smsCmdNum = 0xFF;
	if(!signalQuality(1))
		return 3;
	retSms->smsPending = checkForMessages();
	if(!retSms->smsPending)
	{
		return 0;
	} //no messages
	if(retSms->smsPending == 0xFF)
	{
		retSms->smsPending = 0x00;
		return 1; //there was an error
	} 
	uint8_t l = 1;
	while(retSms->smsPending && (l <= SIMSIZE))
	{
		if(readMessageBreakOut(&sms, l) == 0xFF)
		{
			l++;
			continue;
		}
		deleteMessage(l);
		retSms->smsPending--;
		retSms->smsCmdNum = sms.smsCmdNum;
		strcpy(retSms->smsCmdString,sms.smsCmdString);
		strcpy(retSms->smsNumber,sms.smsNumber);
		char pwd[5];
		EEPROM_readAnything(PINCODE,pwd);
		if(!confirmPassword(sms.smsPwd,pwd))
		{
			retSms->smsDataValid = true;
		}
		else
		{
			retSms->smsDataValid = false;
		}
		return 0;  //message read
	}
	deleteAllMessages(); // too many messages on SIM card
	return 2;
}


bool SIM900::sendATCommandBasic(const char *atCommand, char *buffer, uint8_t arraySize, unsigned long atTimeOut, int smsNumber, boolean YN)
{
	uint8_t index = 0; 
	unsigned long timeOut = millis();
	GSM->print("AT+");
	if (YN)
	{
		GSM->print(atCommand);
		GSM->println(smsNumber);
	}
	else 
		GSM->println(atCommand);
	while ((millis() - timeOut) <= atTimeOut)
	{
		if (GSM->available())
		{
			buffer[index] = GSM->read();
			index++;
			buffer[index] = '\0';
			if((strstr(buffer,atCommand)!=NULL) || (index == (arraySize-1)) ) //look for the AT command, use this to shorten buffer size
				index = 0;
			if(strstr(buffer,ERROR)!=NULL) //if there is an error send AT command again
				return 1;
			if((strstr(buffer,OK)!=NULL) || (strstr(buffer,"NORMAL POWER DOWN") != NULL)) //if there is no error then done
				return 0;
		}
	}
	return 1;
}

/*************************************************************	
	Procedure to check if the GSM module is ready to make and
	receive calls
	RETURN:
		0		GSM is ready
		1		Error, buffer is full
		1		GSM timed out, not ready
**************************************************************/
bool SIM900::callReady()
{
	char rxBuffer[25];
	uint8_t index = 0;
	unsigned long timeOut = millis();
	while (millis() - timeOut <= 30000) //changed from 10 seconds
	{
		if(GSM->available())
		{
			rxBuffer[index] = GSM->read();
			index++;
			rxBuffer[index] = '\0';
		}
		if(strstr(rxBuffer,CALLREADY) != NULL)
			return 0;  //GSM is registered on the network
		if((rxBuffer[index-1] == '\n')||(index == 24))
			index = 0;
	}
	return 1; //GSM is not ready to make/receive calls/sms
}

/*************************************************************	
	Procedure to check the GSM signal quality. The higher the
	number the better the signal quality.
	RETURN:
		0			Error sending AT command/No signal
		1 - 99		RSSI signal strength
		
		
**************************************************************/
uint8_t SIM900::signalQuality(bool wakeUpComm)
{
	if(wakeUpComm)
		gsmSleepMode2();
	char rxBuffer[50];
	uint8_t rssi = 0;
	if(sendATCommandBasic(CSQ,rxBuffer,50,CSQ_TO,0,false)){return 0;}
	char *ptr = rxBuffer;
	char *str = NULL;
	ptr = strtok_r(ptr," ",&str);
	ptr = strtok_r(NULL,",",&str);
	rssi = atoi(ptr);  
	return rssi;
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

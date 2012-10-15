#include "SIM900.h"

prog_char IPR[] PROGMEM 	= "IPR=9600";  	//basic
prog_char CMGR[] PROGMEM 	= "CMGR="; 		//ext
prog_char CMGD[] PROGMEM 	= "CMGD=";		//basic
prog_char CPMS[] PROGMEM 	= "CPMS?";		//ext
prog_char CMGF[] PROGMEM 	= "CMGF=1";		//basic
prog_char CNMI[] PROGMEM 	= "CNMI=0,0,0,0,0";	//basic
prog_char CREG[] PROGMEM 	= "CREG?";		//ext
prog_char CSCLK[] PROGMEM 	= "CSCLK=";		//basic
prog_char CPOWD[] PROGMEM 	= "CPOWD=1";	//
prog_char CMGDA[] PROGMEM 	= "CMGDA=\"DEL ALL\"";	//basic
prog_char CMEE[] PROGMEM 	= "CMEE=1"; 	//basic
prog_char CSQ[] PROGMEM 	= "CSQ"; 		//ext


const char * myArray[] PROGMEM =
{
    IPR,	
	CMGR,
	CMGD,
	CPMS,
	CMGF,
	CNMI,
	CREG,
	CSCLK,
	CPOWD,
	CMGDA,
	CMEE,
	CSQ
};


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

void SIM900::printPROGMEM(int index, int atArg)
{
    unsigned int flash_address = pgm_read_word(&myArray[index]);
    char c = 0;
	do {
      c = (char) pgm_read_byte(flash_address++);
	  if(c == '\0')
		break;
      GSM->print(c);
    } while (c!='\0');
	if(atArg == 0xFF)
		GSM->println();
	else
		GSM->println(atArg);
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

uint8_t SIM900::gsmSleepMode0()
{
	GSM->println("AT\r\nAT");
	delay(50);
	if(!atNoData(7,15,CSCLK_TO,0))
		return 0; //CSCLK0
	return 1;
}

uint8_t SIM900::gsmSleepMode2()
{
	GSM->println("AT\r\nAT");
	if(!atNoData(7,15,CSCLK_TO,2))
		return 0; //CSCLK2
	return 1;
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
	atNoData(10,15,CMEE_TO); //CMEE
	atNoData(0,15,IPR_TO); //IPR
	atNoData(4,15,CMGF_TO); //CMGF
	atNoData(5,22,CNMI_TO); //CNMI
}

uint8_t SIM900::atNoData(uint8_t atCmd, uint8_t arraySize, unsigned long atTimeOut, int atArg)
{
	uint8_t oe;
	uint8_t index = 0;
	unsigned long timeOut;
	char rxBuffer[arraySize];
	GSM->flush();
	GSM->print("AT+");
	printPROGMEM(atCmd, atArg);
	timeOut = millis();
	while((millis() - timeOut) <= atTimeOut)
	{
		if (GSM->available())
		{
			rxBuffer[index] = GSM->read();
			index++;
			rxBuffer[index] = '\0';
			oe = okError(rxBuffer);
			if(!oe)
				return 0;  // OK found
			else if(oe == 1)
				break;  // ERROR found
			if(rxBuffer[index-1] == '\n')
				index = 0; 
			if(index >= (arraySize-1))
			{
				index = 0;
				rxBuffer[index] = '\0';
				continue;
			}
		}
	}
	return 3; //timed out
}

uint8_t SIM900::okError(char *buffer)
{
	if(strstr(buffer,"OK") != NULL)
		return 0;
	if(strstr(buffer,"ERROR") != NULL)
		return 1;
	return 2; //nothing found
}

uint8_t SIM900::checkNetworkRegistration()
{
	char rxBuffer[50];
	sendATCommandBasic(CREG,rxBuffer,50,CREG_TO,0,false);  //check network registration status
	if(strstr(rxBuffer,",1") != NULL || strstr(rxBuffer,",5") != NULL)
		return 0; //GSM is registered on the network
	return 1; //GSM is not registered on the network
}

uint8_t SIM900::checkForMessages()
{
	char rxBuffer[75]; //was 100
	if(sendATCommandBasic(CPMS,rxBuffer,75,CPMS_TO,0,false)){return 0xFF;}  //check for new messages
	char *ptr = rxBuffer;
	char *str = NULL;
	uint8_t receivedMessages = 0;  //total unread messages stored on the SIM card
	ptr = strtok_r(ptr,",",&str);
	ptr = strtok_r(NULL,",",&str);
	receivedMessages = atoi(ptr);  //Number of messages on the sim card
	return receivedMessages;
}

uint8_t SIM900::deleteMessage(uint8_t msg)
{
	if(atNoData(2,15,CMGD_TO,msg))
		return 1; //CMGD
	return 0;
}

uint8_t SIM900::deleteAllMessages()
{
	if(atNoData(9,22,CMGDA_TO))
		return 1; //CMGDA
	return 0;
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
		ptr = strtok_r(NULL,"-",&str);
		strcpy(sms->smsTime,ptr);
		ptr = strtok_r(NULL,"\n",&str);
	}	
	else if(replyMessageType == 2) //message is an email
	{
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,"\"",&str);ptr = strtok_r(NULL,"\"",&str);
		ptr = strtok_r(NULL,",",&str);
		strcpy(sms->smsDate,ptr);
		ptr = strtok_r(NULL,"-",&str);
		strcpy(sms->smsTime,ptr);
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
		switch(addressType)
		{
			case 0:
				GSM->print("AT+CMGS=\"");
				GSM->print(smsAddress);
				GSM->println("\"");
				break;
			case 1:
				{
				unsigned long apn;
				EEPROM_readAnything(APN,apn);
				GSM->print("AT+CMGS=\"");
				GSM->print(apn,DEC);
				GSM->println("\"");
				}
				break;
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
			GSM->println(0x1B,BYTE); //do not send message
			delay(500);
			GSM->flush();
			return 1;  //There was an error waiting for the > 
		} 
		if(addressType == 1)
		{
			GSM->println(smsAddress);
		}
		
		if(!smsFormat){return 0;} //Header information successfully sent
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
    GSM->println(0x1A,BYTE);
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
			if(strstr(rxBuffer,"ERROR")!= NULL)
			{
//				GSM->flush();
				return 2;  // There was an error sending the SMS/email
			}
			if(strstr(rxBuffer,"OK")!= NULL)
			{
//				GSM->flush();
				return 0;  // SMS/email successfully sent
			}
			if(rxBuffer[idx-1] == '\n')
				idx = 0;
			if (idx == 19) //Buffer is full, there was a problem
				return 4;
		}
	}
//	GSM->flush();
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


uint8_t SIM900::sendATCommandBasic(prog_char *progmemAtCommand, char *buffer, uint8_t arraySize, unsigned long atTimeOut, int smsNumber, boolean YN)
{
	uint8_t index = 0; 
	char atCommand[16];// was 16
	unsigned long timeOut = millis();
	strcpy_P(atCommand,progmemAtCommand);
	GSM->flush();
	GSM->print("AT+");
	if (YN)
	{
		GSM->print(atCommand);
		GSM->println(smsNumber);
	}
	else 
	{
		GSM->println(atCommand);
	}
	while ((millis() - timeOut) <= atTimeOut)
	{
		if (GSM->available())
		{
			buffer[index] = GSM->read();
			index++;
			buffer[index] = '\0';
			if(strstr(buffer,atCommand)!=NULL) //look for the AT command, use this to shorten buffer size
			{
				index = 0;
				buffer[index] = '\0';
			}
			if(strstr(buffer,"ERROR")!=NULL) //if there is an error send AT command again
				return 1;
			if((strstr(buffer,"OK")!=NULL) || (strstr(buffer,"NORMAL POWER DOWN") != NULL)) //if there is no error then done
				return 0;
			if (index == (arraySize-1)) //Buffer is full, there was a problem
				return 2;
		}
	}
	return 3;
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
		if(strstr(rxBuffer,"Call Ready") != NULL)
		{
			return 0;  //GSM is registered on the network
		}
		if(rxBuffer[index-1] == '\n')
				index = 0;
		if (index == 24) //Buffer is full, there was a problem
					return 1;
	}
	return 2; //GSM is not ready to make/receive calls/sms
}

/*************************************************************	
	Procedure to check if the SIM card is inserted
	RETURN:
		0		SIM card present
		1		SIM card not present
		2		Error, buffer full
		3		Time out
**************************************************************/
uint8_t SIM900::cpin()
{
	char rxBuffer[25];
	uint8_t index = 0;
	unsigned long timeOut = millis();
	while (millis() - timeOut <= CPIN_TO) 
	{
		if(GSM->available())
		{
			rxBuffer[index] = GSM->read();
			index++;
			rxBuffer[index] = '\0';
		}
		if(strstr(rxBuffer,"READY") != NULL)
		{
			return 0;  //SIM card present
		}
		if(strstr(rxBuffer,"INSERTED") != NULL)
		{
			return 1;  //SIM card not present
		}
		if(rxBuffer[index-1] == '\n')
			index = 0;
		if (index == 24) //Buffer is full, there was a problem
			return 2;
	}
	return 3; //Timed out
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
	GSM->print(*lat/1000000);
	GSM->print("+");
	GSM->print(abs((*lat%1000000)/10000));
	GSM->print(".");
	GSM->print(abs(*lat%10000));
	GSM->print(",");
	GSM->print(*lon/1000000);
	GSM->print("+");
	GSM->print(abs((*lon%1000000)/10000));
	GSM->print(".");
	GSM->print(abs(*lon%10000));
}


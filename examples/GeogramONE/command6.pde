void command6()
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	uint16_t eepAdd = atoi(ptr);
	switch(eepAdd)
	{//uint8_t
		case RETURNADDCONFIG: case BATTERYLOWLEVEL: case IOSTATE0: case IOSTATE1: 
		case IOSTATE2: case IOSTATE3: case IOSTATE4: case IOSTATE5: case IOSTATE6: case IOSTATE7: 
		case IOSTATE8: case IOSTATE9: case ACTIVE1: case ACTIVE2: case ACTIVE3: 
		case INOUT1: case INOUT2: case INOUT3: case BMA0X0F: case BMA0X10: case BMA0X11: 
		case BMA0X16: case BMA0X17: case BMA0X19: case BMA0X1A: case BMA0X1B: case BMA0X20: case BMA0X21: 
		case BMA0X25: case BMA0X26: case BMA0X27: case BMA0X28: case DATEFORMAT: case ENGMETRIC: case SLEEPTIMECONFIG:
		case BREACHSPEED: case BREACHREPS: case SPEEDHYST: case UDPPOWERPROFILE: case SMSPOWERPROFILE:
		case UDPSPEEDBAT: case UDPSPEEDPLUG: case SMSSPEEDBAT: case SMSSPEEDPLUG:
 			ptr = strtok_r(NULL,".",&str);
			EEPROM.write(eepAdd,(uint8_t)atoi(ptr));
			break;
	//int8_t
		case TIMEZONE:
			ptr = strtok_r(NULL,".",&str);
			EEPROM.write(eepAdd,(int8_t)atoi(ptr));
			break;
	//uint16_t
		case GEODATAFORMAT1: case GEODATAFORMAT2: case SPEEDLIMIT: case GPRS_PORT:
			ptr = strtok_r(NULL,".",&str);
			EEPROM_writeAnything(eepAdd,(uint16_t)(atoi(ptr)));
			break;
	//unsigned long 
		case RADIUS1: case RADIUS2: case RADIUS3: case SLEEPTIMEON:
		case SLEEPTIMEOFF: case SMSSENDINTERVALBAT: case SMSSENDINTERVALPLUG: 
		case UDPSENDINTERVALBAT: case UDPSENDINTERVALPLUG: 
			ptr = strtok_r(NULL,".",&str);
			EEPROM_writeAnything(eepAdd,(unsigned long)(atol(ptr)));
			break;
	// long
		case LATITUDE1: case LATITUDE2: case LATITUDE3: case LONGITUDE1: case LONGITUDE2: case LONGITUDE3:
			ptr = strtok_r(NULL,".",&str);
			EEPROM_writeAnything(eepAdd,(long)(atol(ptr)));
			break;
	//string length 4 characters...not including terminating null
		case PINCODE:
			ptr = strtok_r(NULL,".",&str);
			writeEEPROM(ptr,eepAdd,4);
			break;
	//string length 38 characters...not including terminating null
		case SMSADDRESS:
			ptr = strtok_r(NULL,".",&str);
			writeEEPROM(ptr,eepAdd,38);
			break;
	//string length 24 characters...not including terminating null
		case MOTIONMSG: case BATTERYMSG: case FENCE1MSG: case FENCE2MSG: case FENCE3MSG: case SPEEDMSG: case MAXSPEEDMSG: case GEOGRAMONEID:
		case D4MSG: case D10MSG: case GPRS_USER: case GPRS_PASS:
			ptr = strtok_r(NULL,"..",&str); //changed to double period
			writeEEPROM(ptr,eepAdd,24);
			break;
	//string length 49 characters...not including terminating null
		case HTTP1: case HTTP2: case HTTP3: case GPRS_APN:
			ptr = strtok_r(NULL,"..",&str); //changed to double period
			writeEEPROM(ptr,eepAdd,49);
			break;
	//string length 15 characters...not including terminating null
		case IMEI: case GPRS_HOST:
			ptr = strtok_r(NULL,"..",&str);
			writeEEPROM(ptr,eepAdd,15);
			break;
	//string length 10 characters...not including terminating null
		case UDP_HEADER: case UDP_REPLY: 
			ptr = strtok_r(NULL,"..",&str); //changed to double period from colon
			writeEEPROM(ptr,eepAdd,10);
			break;
			
	}
}

void writeEEPROM(char *eptr, uint16_t eAddress, uint8_t eSize)
{
	for(uint8_t e = 0; e < eSize; e++)
	{
		EEPROM.write(eAddress + e,eptr[e]);
		if(eptr[e] == NULL)
			break;
	}
	EEPROM.write(eAddress + eSize,'\0'); 
}
      

uint8_t command6()
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	uint16_t eepAdd = atoi(ptr);
	switch(eepAdd)
	{ //byte length
		case 87: case 88: case 89: case IOSTATE0: case IOSTATE1: case IOSTATE2: case IOSTATE3:
		case IOSTATE4: case IOSTATE5: case IOSTATE6: case IOSTATE7: case IOSTATE8: case IOSTATE9:
		case ACTIVE1: case ACTIVE2: case ACTIVE3: case RESET1: case RESET2: case RESET3: case INOUT1:
		case INOUT2: case INOUT3:
			ptr = strtok_r(NULL,".",&str);
			EEPROM.write(eepAdd,atoi(ptr));
			break;
	//long length
		case APN: case RADIUS1: case RADIUS2: case RADIUS3:
			ptr = strtok_r(NULL,".",&str);
			EEPROM_writeAnything(eepAdd,(unsigned long)(atol(ptr)));
			break;
		case PINCODE:
			ptr = strtok_r(NULL,".",&str);
			for(uint8_t e = 0; e < 4; e++)
			{
				EEPROM.write(eepAdd + e,ptr[e]);
			}
			EEPROM.write(eepAdd + 4,'\0'); 
			break;
		case SMSADDRESS: case EMAILADDRESS:
			ptr = strtok_r(NULL,".",&str);
			for(uint8_t e = 0; e < 38; e++)
			{
				EEPROM.write(eepAdd + e,ptr[e]);
				if(ptr[e] == NULL)
					break;
			}
			EEPROM.write(eepAdd + 38,'\0'); 
			break;
		case MOTIONMSG: case BATTERYMSG: case FENCE1MSG: case FENCE2MSG: case FENCE3MSG: case SPEEDMSG:
			ptr = strtok_r(NULL,".",&str);
			for(uint8_t e = 0; e < 24; e++)
			{
				EEPROM.write(eepAdd + e,ptr[e]);
				if(ptr[e] == NULL)
					break;
			}
			EEPROM.write(eepAdd + 24,'\0'); 
			break;
	}
}
      

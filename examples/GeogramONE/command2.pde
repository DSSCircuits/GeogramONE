// Example: Set and Activate Fence1, Inside fence, radius 100 ft     xxxx.2.1.1.0.100.
// Example: Deactivate Fence1                                        xxxx.2.10.
// Example: Activate Fence3 with what is stored in EEPROM            xxxx.2.31.
// Example: Set and Deactivate Fence2, Outside fence, radius 350 ft  xxxx.2.3.0.1.350.

void command2()
{
	uint8_t cmd;
	uint8_t initFence = 1;
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str); 
	cmd = atoi(ptr); //fence 1,2 or 3
	switch(cmd)
	{
		case 1 :
		case 2 :
		case 3 :
		{
			uint8_t offset = 0;
			if(cmd == 2)
			{
				offset = 14;
				initFence = 2;
			}
			if(cmd == 3)
			{
				offset = 28;
				initFence = 3;
			}
			ptr = strtok_r(NULL,".",&str); 
			cmd = atoi(ptr) & 0x01; // deactivate 0 or activate 1
			EEPROM.write(ACTIVE1 + offset,cmd);
			ptr = strtok_r(NULL,".",&str);
			cmd = atoi(ptr) & 0x01; // inside fence 0 or outside fence 1
			EEPROM.write(INOUT1 + offset,cmd);
			ptr = strtok_r(NULL,".",&str);
			unsigned long cmdLong = atol(ptr); // fence radius
			EEPROM_writeAnything(RADIUS1 + offset, cmdLong);
			EEPROM_writeAnything(LATITUDE1 + offset, lastValid.latitude);
			EEPROM_writeAnything(LONGITUDE1 + offset, lastValid.longitude);
			}
			break;
		case 10 :
			EEPROM.write(ACTIVE1,0);
			break;
		case 11 :
			EEPROM.write(ACTIVE1,1);
			break;
		case 20 :
			EEPROM.write(ACTIVE2,0);
			initFence = 2;
			break;
		case 21 :
			EEPROM.write(ACTIVE2,1);
			initFence = 2;
			break;
		case 30 :
			EEPROM.write(ACTIVE3,0);
			initFence = 3;
			break;
		case 31 :
			EEPROM.write(ACTIVE3,1);
			initFence = 3;
			break;
		default :
			return;
			break;
	}
	if(initFence == 1)
		ggo.getFenceActive(1, &fence1); 
	if(initFence == 2)
		ggo.getFenceActive(2, &fence2); 
	if(initFence == 3)
		ggo.getFenceActive(3, &fence3);
}


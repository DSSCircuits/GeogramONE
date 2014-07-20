void command7()  //toggle IO pins
{
	char *ptr = NULL;
	char *str = NULL;
	uint8_t ioChannel;
	uint8_t ioState;
	uint8_t ioAddress;
	uint16_t ioData;
	uint16_t toggleDelay = 0;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	ioChannel = atoi(ptr);
	switch(ioChannel)
	{
		case 0:
			ioState = EEPROM.read(IOSTATE0);
			ioAddress = 4;
			break;
		case 1:
			ioState = EEPROM.read(IOSTATE1);
			ioAddress = 10;
			break;
		case 2:
			ioState = EEPROM.read(IOSTATE2);
			ioAddress = 15; 
			break;
		case 3:
			ioState = EEPROM.read(IOSTATE3);
			ioAddress = 16;
			break;
		case 4:
			ioState = EEPROM.read(IOSTATE4);
			ioAddress = 17;
			break;
		case 5:
			ioState = EEPROM.read(IOSTATE5);
			ioAddress = 20;
			break;
		default:
			return;
			break;
	}
	switch(ioState)
	{
		case 0:
		case 1:
			ioData = digitalRead(ioAddress);
			break;
		case 2:
		case 3:
			ptr = strtok_r(NULL,".",&str);
			switch(atoi(ptr))
			{
				case 2:
					toggleDelay = atoi(strtok_r(NULL,".",&str));
				case 0:
					digitalWrite(ioAddress,0);
					if(toggleDelay)
					{
						delay(toggleDelay * 10);
						digitalWrite(ioAddress,1);
					}
					break;
				case 1:
					digitalWrite(ioAddress,1);
					if(toggleDelay)
					{
						delay(toggleDelay * 10);
						digitalWrite(ioAddress,0);
					}
					break;
			}
			return;
			break;
		case 4:
			ioData = analogRead(ioAddress - 14);
			break;
		default :
			return;
	}
	goesWhere(smsData.smsNumber,3);
	if(!sim900.prepareSMS(smsData.smsNumber,apn))
	{
		GSM.print("IO Channel ");GSM.print(ioChannel,DEC);
		GSM.print(" = ");GSM.println(ioData,DEC);
	}
	sim900.sendSMS();
}

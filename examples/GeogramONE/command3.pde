
void command3() //speed monitoring mode
{
	static uint16_t maxSpeed = 0;
	if(cmd3 == 1)
	{
		char *ptr = NULL;
		char *str = NULL;
		ptr = strtok_r(smsData.smsCmdString,".",&str);
		speedLimit = atoi(ptr);
		EEPROM_writeAnything(SPEEDLIMIT,speedLimit);
		if(!speedLimit)
			cmd3 = 0;
		else
			cmd3 = 0x02;
	}
	if((cmd3 == 2) && (lastValid.speedKnots > speedLimit))
		cmd3 = 0x03;
	if(cmd3 == 3)
	{
		if(sim900.sendMessage(0,smsData.smsNumber,NULL))
			return;
		char eepChar;
		for (uint8_t ep = 0; ep < 25; ep++)
		{
			eepChar = EEPROM.read(ep + SPEEDMSG);
			if(eepChar == '\0')
				break;
			else
				GSM.print(eepChar);
		}
		GSM.println();
		uint16_t speedDataOnly = 0x4818; //Speed, course, battery percent, ID
		printHTTP(&speedDataOnly, 0);
		if(sim900.sendMessage(3,NULL,NULL))
			return;
		maxSpeed = lastValid.speedKnots;
		cmd3 = 0x04;
	}
	if(cmd3 == 4)
	{
		if(lastValid.speedKnots > maxSpeed)
			maxSpeed = lastValid.speedKnots;
		if(lastValid.speedKnots <= (uint16_t)(speedLimit - speedHyst))
			cmd3 = 0x05;
	}
	if(cmd3 == 5)
	{
		if(sim900.sendMessage(0,smsData.smsNumber,NULL))
			return;
		char eepChar;
		for (uint8_t ep = 0; ep < 25; ep++)
		{
			eepChar = EEPROM.read(ep + MAXSPEEDMSG);
			if(eepChar == '\0')
				break;
			else
				GSM.print(eepChar);
		}
		GSM.println(maxSpeed);
		uint16_t speedDataOnly = 0x4818; //Speed, course, battery percent, ID
		printHTTP(&speedDataOnly, 0);
		if(sim900.sendMessage(3,NULL,NULL))
			return;
		cmd3 = 0x02; 
		maxSpeed = 0;
	}
}

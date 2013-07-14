
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
	if((cmd3 == 2) && (lastValid.speed > speedLimit))
		cmd3 = 0x03;
	if(cmd3 == 3)
	{
		goesWhere(smsData.smsNumber);
		if(sim900.prepareSMS(smsData.smsNumber))
			return;
		printEEPROM(SPEEDMSG);
		GSM.println();
		uint16_t speedDataOnly = 0x4818; //Speed, course, battery percent, ID
		printHTTP(&speedDataOnly, 0);
		if(sim900.sendSMS())
			return;
		maxSpeed = lastValid.speed;
		cmd3 = 0x04;
	}
	if(cmd3 == 4)
	{
		if(lastValid.speed > maxSpeed)
			maxSpeed = lastValid.speed;
		if(lastValid.speed <= (uint16_t)(speedLimit - speedHyst))
			cmd3 = 0x05;
	}
	if(cmd3 == 5)
	{
		goesWhere(smsData.smsNumber);
		if(sim900.prepareSMS(smsData.smsNumber))
			return;
		printEEPROM(MAXSPEEDMSG);
		GSM.println(maxSpeed);
		uint16_t speedDataOnly = 0x4818; //Speed, course, battery percent, ID
		printHTTP(&speedDataOnly, 0);
		if(sim900.sendSMS())
			return;
		cmd3 = 0x02; 
		maxSpeed = 0;
	}
}

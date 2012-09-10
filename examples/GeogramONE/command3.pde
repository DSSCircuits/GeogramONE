
uint8_t command3(uint8_t *cmd3) //speed monitoring mode
{
	char atCommand[32];
	static uint16_t speedLimit = 0;
	static uint16_t maxSpeed = 0;
	if(*cmd3 == 1)
	{
		char *ptr = NULL;
		char *str = NULL;
		ptr = strtok_r(smsData.smsCmdString,".",&str);
		speedLimit = atoi(ptr);
		if(!speedLimit)
			*cmd3 = 0;
		else
			*cmd3 = 0x02;
	}
	if((*cmd3 == 2) && (lastValid.speedKnots > speedLimit))
		*cmd3 = 0x03;
	if(*cmd3 == 3)
	{
		strcpy_P(atCommand,googlePrefix);
		if(sim900.sendMessage(0,smsData.smsNumber,NULL))
			return 1;
		GSM.println("Speed Alert");
		GSM.print(atCommand);
		sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
		GSM.print("+(");
		GSM.print("Me");
		strcpy_P(atCommand,googleSuffix);
		GSM.println(atCommand);
		if(sim900.sendMessage(3,NULL,NULL))
			return 1;
		maxSpeed = lastValid.speedKnots;
		*cmd3 = 0x04;
	}
	if(*cmd3 == 4)
	{
		if(lastValid.speedKnots > maxSpeed)
			maxSpeed = lastValid.speedKnots;
		if(lastValid.speedKnots <= (uint16_t)(speedLimit - 3))
			*cmd3 = 0x05;
	}
	if(*cmd3 == 5)
	{
		strcpy_P(atCommand,googlePrefix);
		if(sim900.sendMessage(0,smsData.smsNumber,NULL))
			return 1;
		GSM.print("Max Speed ");
		GSM.println(maxSpeed);
		GSM.print(atCommand);
		sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
		GSM.print("+(");
		GSM.print("Me");
		strcpy_P(atCommand,googleSuffix);
		GSM.println(atCommand);
		if(sim900.sendMessage(3,NULL,NULL))
			return 1;
		*cmd3 = 0x02; 
		maxSpeed = 0;
	}
}

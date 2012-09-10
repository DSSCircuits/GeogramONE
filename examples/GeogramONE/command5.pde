uint8_t command5(uint8_t *cmd5, volatile uint8_t *opt1, volatile uint8_t *opt2, volatile uint8_t *opt3)  
{
	static uint32_t timeOn = 0;
	static uint32_t timeOff = 0;
	static unsigned long onOffTimer = millis();
	if( *cmd5 == 1 )
	{
		char *ptr = NULL;
		char *str = NULL;
		ptr = strtok_r(smsData.smsCmdString,".",&str);
		timeOn = atol(ptr) * 1000UL;
		ptr = strtok_r(NULL,".",&str);
		timeOff = atol(ptr) * 1000UL;
		if(!timeOn || !timeOff)
			*cmd5 = 0;
		else
			*cmd5 = 2;
	}
	if((*cmd5 == 2) && (!opt1) && (!opt2) && (!opt3)) //reset on timer
	{
		onOffTimer = millis();
		*cmd5 = 3;
	}
	if((*cmd5 == 3) && (!opt1) && (!opt2) && (!opt3) && ((millis() - onOffTimer) > timeOn))
	{
		gps.sleepGPS();
		sim900.powerDownGSM();
		bma250.anyMotion();
		*cmd5 = 4;
		onOffTimer = millis();
	}
	if( (*cmd5 == 4) && (!opt1) && (!opt2) && (!opt3))
	{
		while((millis() - onOffTimer) < timeOff)
		{
			if(opt1 && opt2 && opt3)
			break;
		}
		*cmd5 = 5;
	}
	if(*cmd5 == 5)
	{
		gps.wakeUpGPS();
		sim900.init(9600);
		bma250.disableInterrupts();
		*cmd5 = 2;
	}
}



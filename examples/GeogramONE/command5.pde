void command5()  
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	sleepTimeOn = atol(ptr) * 1000UL;
	ptr = strtok_r(NULL,".",&str);
	sleepTimeOff = atol(ptr) * 1000UL;
	ptr = strtok_r(NULL,".",&str);
	sleepTimeConfig = atoi(ptr) & 0x0F;
	EEPROM_writeAnything(SLEEPTIMEON,sleepTimeOn);
	EEPROM_writeAnything(SLEEPTIMEOFF,sleepTimeOff);
	EEPROM_writeAnything(SLEEPTIMECONFIG,sleepTimeConfig);
}

void sleepTimer() 
{		
	static unsigned long onOffTimer = millis();
	if((millis() - onOffTimer) < (sleepTimeOn))
		return;
	if((sleepTimeConfig & 0x04) && call) //there is a message waiting do not go to sleep
	{
		onOffTimer = millis();
		return;
	}
	if((sleepTimeConfig & 0x02) && (move & 0x01)) //there was recent movement do not go to sleep
	{
		onOffTimer = millis();
		move &= ~(0x01); 
		return;
	}	
	if((sleepTimeConfig & 0x01) && charge) //unit is plugged in and charging do not go to sleep
	{
		onOffTimer = millis();
		return;
	}
	if(sleepTimeConfig & 0x08) //sleep GO and wait for movement
	{
		sim900.powerDownGSM();
		gps.sleepGPS();
		BMA250enableInterrupts();
		BMA250sleepMode(0x5A);
		MAX17043sleep(false);
		ggo.goToSleep(SLEEP_MODE_PWR_DOWN,(bool)(~(sleepTimeConfig) & 0x01), true ); //if charge override enabled, wake device when plugged in
		MAX17043sleep(true);
		BMA250sleepMode(0x00);
		gps.wakeUpGPS();
		sim900.initializeGSM();
		gsmPowerStatus = true;
		onOffTimer = millis();
		move &= ~(0x01);
		return;
	}
	gps.sleepGPS();
	if(!(sleepTimeConfig & 0x04))
	{
		sim900.powerDownGSM();
		gsmPowerStatus = false;
	}
	onOffTimer = millis();
	while((millis() - onOffTimer) < sleepTimeOff)
	{
		if((sleepTimeConfig & 0x04) && call)
			break;
		if((sleepTimeConfig & 0x02) && (move & 0x01))
			break;
		if((sleepTimeConfig & 0x01) && charge)
			break;
	}
	gps.wakeUpGPS();
	if(!(sleepTimeConfig & 0x04))
	{
		sim900.initializeGSM();
		gsmPowerStatus = true;
	}
	onOffTimer = millis();
}
		

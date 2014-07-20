
void command1() //motion sensing mode
{
	switch(cmd1)
	{
		case 0x01 :
			sim900.powerDownGSM();
			gps.sleepGPS();
			BMA250enableInterrupts();
			BMA250sleepMode(0x5A);
			MAX17043sleep(false);
			ggo.goToSleep(SLEEP_MODE_PWR_DOWN, true, true);
			gps.wakeUpGPS();
			sim900.initializeGSM();
			MAX17043sleep(true);
			BMA250sleepMode(0x00);
			gsmPowerStatus = true;
			cmd1 = 0x02;
			break;
		case 0x02 :
			goesWhere(smsData.smsNumber,1);
			if(!sim900.prepareSMS(smsData.smsNumber,apn))
			{
				printEEPROM(MOTIONMSG);
				if(!sim900.sendSMS())
					cmd1 = 0;
			}
			break;
	}
}

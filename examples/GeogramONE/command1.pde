
void command1() //motion sensing mode
{
	switch(cmd1)
	{
		case 0x01 :  
			gps.sleepGPS();
			sim900.powerDownGSM();
			BMA250enableInterrupts();
			ggo.goToSleep();
			gps.wakeUpGPS();
			sim900.init(9600);
			if(!(sleepTimeConfig & 0x02))
				BMA250disableInterrupts();
			cmd1 = 0x02;
			break;
		case 0x02 :
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(MOTIONMSG);
				if(!sim900.sendSMS())
					cmd1 = 0;
			}
			break;
	}
}

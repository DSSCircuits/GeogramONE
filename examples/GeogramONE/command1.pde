
uint8_t command1(uint8_t *cmd1) //motion sensing mode
{
	switch(*cmd1)
	{
		case 0x01 :  
			gps.sleepGPS();
			sim900.powerDownGSM();
		//	bma250.anyMotion();
			bma250.enableInterrupts();
			ggo.goToSleep();
			gps.wakeUpGPS();
			sim900.init(9600);
			bma250.disableInterrupts();
			*cmd1 = 0x02;
			break;
		case 0x02 :
			if(sim900.sendMessage(2,smsData.smsNumber,NULL,MOTIONMSG))
				return 1;
			*cmd1 = 0;
			break;
	}
}

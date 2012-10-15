
void command1() //motion sensing mode
{
	switch(cmd1)
	{
		case 0x01 :  
			gps.sleepGPS();
			sim900.powerDownGSM();
			bma250.enableInterrupts();
			ggo.goToSleep();
			gps.wakeUpGPS();
			sim900.init(9600);
			if(!(sleepTimeConfig & 0x02))
				bma250.disableInterrupts();
			cmd1 = 0x02;
			break;
		case 0x02 :
			if(!sim900.sendMessage(2,smsData.smsNumber,NULL,MOTIONMSG))
				cmd1 = 0;
			break;
	}
}

void udpOrange()
{
	static bool sendOK = false;
	if(!lastValid.signalLock)
		return;
	if(!(lastValid.updated & 0x01))
		return;
	if(udpInterval > 5)
		sim900.gsmSleepMode(0);
	if(!sendOK)	
	{
		for(uint8_t g = 0; g < 11; g++)
		{
			udpReply[g] = EEPROM.read(UDP_REPLY + g);
		}
		GSM.println("AT+CGATT?");
		if(!sim900.confirmAtCommand(": 0",3000))
		{
			sim900.confirmAtCommand("OK",500);
			if(!sim900.signalQuality())
				return;
			if(!sim900.checkNetworkRegistration())
			{
				GSM.println("AT+CGATT=1");	
				if(sim900.confirmAtCommand("OK",10000) == 1) //if ERROR, need to reboot GSM module
				{
					static unsigned long resetGSM = millis();
					if((millis() - resetGSM) >= 300000) //if more than 5 minutes reboot GSM module
					{
						sim900.powerDownGSM();
						sim900.init(9600);
						resetGSM = millis();
					}
				}
			}
			else
				return;
		}
		uint8_t cStatus = sim900.cipStatus();
		switch(cStatus)
		{
			case 0:
				GSM.print("AT+CSTT=\"");
				printEEPROM(GPRS_APN);
				GSM.println("\"");
				if(sim900.confirmAtCommand("OK",3000))
					return;
			case 1:
				GSM.println("AT+CIICR");
				if(sim900.confirmAtCommand("OK",5000))
					return;
			case 2:
			//	return; //might need to put return back in
			case 3:
				GSM.println("AT+CIFSR");
				if(sim900.confirmAtCommand(".",2000) == 1)
					return;
				sim900.confirmAtCommand("\r\n",100);
			case 4:
			{
				GSM.print("AT+CIPSTART=\"UDP\",\"");
				printEEPROM(GPRS_HOST);
				GSM.print("\",\"");
				uint16_t portNumber = 0;
				EEPROM_readAnything(GPRS_PORT,portNumber);
				GSM.print(portNumber,DEC);
				GSM.println("\"");
				if(sim900.confirmAtCommand("T OK",2000))
					return;
			}
			case 5:
				break; //might need to change to return because of transition between 5 and 6
			case 6:
				break;
			case 7:
			case 8:
			case 9:
				GSM.println("AT+CIPSHUT");
				sim900.confirmAtCommand("OK",3000);
				return;
			default:
				return;
		}
	}
	GSM.println("AT+CIPSEND");
	if(!sim900.confirmAtCommand(">",3000))
	{
		printEEPROM(IMEI);
		printEEPROM(UDP_HEADER);
		GSM.print(lastValid.date);
	//	GSM.print("NA");
		GSM.print(";");
		GSM.print(lastValid.time);
	//	GSM.print("NA");
		GSM.print(";");
		GSM.print(lastValid.latitude);
		GSM.print(";");
		GSM.print(lastValid.ns);
		GSM.print(";");
		GSM.print(lastValid.longitude);
		GSM.print(";");
		GSM.print(lastValid.ew);
		GSM.print(";");
		GSM.print(lastValid.speed);
		GSM.print(";");
		GSM.print(lastValid.course);
		GSM.print(";");
		GSM.print(lastValid.altitude);
		GSM.print(";");
	//	GSM.println(lastValid.satellitesUsed);
		GSM.println("NA");
		GSM.println((char)0x1A);
		if(sim900.confirmAtCommand("OK\r\n",3000))
		{
			sendOK = false;
			return;
		}
		sendOK = true;
		if(!sim900.confirmAtCommand(udpReply,UDPREPLY_TO))
		{
			udp = 0;
			lastValid.updated &= ~(0x01);
			sendOK = true;
			if(udpInterval > 5)
				sim900.gsmSleepMode(2);
			return;
		}
	}
	else
	{
		sendOK = false;
		return;
	}
}

void udpOrange()
{
	if(!lastValid.signalLock)
		return;
	if(gprsInterval > 5)
		sim900.gsmSleepMode(0);
	GSM.println("AT+CGATT?");
	if(sim900.confirmAtCommand(": 1",3000))
		return;
	GSM.println("AT+CIPSEND");
	if(!sim900.confirmAtCommand(">",3000))
	{
		sim900.printEEPROM(IMEI);
		sim900.printEEPROM(GPRS_HEADER);
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
		GSM.print(lastValid.speed * KNOTSTOKPH);
		GSM.print(";");
		GSM.print(lastValid.course);
		GSM.print(";");
		GSM.print(lastValid.altitude);
		GSM.print(";");
	//	GSM.println(lastValid.satellitesUsed);
		GSM.println("NA");
		GSM.println(0x1A,BYTE);
		sim900.confirmAtCommand("\r\n",3000);
		if(!sim900.confirmAtCommand(gprsReply,3000))
		{
			udp = 0;
			if(gprsInterval > 5)
				sim900.gsmSleepMode(2);
			return;
		}
	}
	GSM.println("AT+CIPSTATUS");
	if(!sim900.confirmAtCommand("P DEACT",3000))
	{
		GSM.println("AT+CIPSHUT");
		sim900.confirmAtCommand("OK",3000);
		GSM.print("AT+CSTT=\"");
		sim900.printEEPROM(GPRS_APN);
		GSM.println("\"");
		sim900.confirmAtCommand("OK",3000);
		GSM.println("AT+CIICR");
		sim900.confirmAtCommand("OK",5000);
		delay(100);
		GSM.println("AT+CIFSR");
		sim900.confirmAtCommand("OK",2000);
		return;
	}
	udp = 1;
	for(uint8_t g = 0; g < 11; g++)
	{
		gprsReply[g] = EEPROM.read(GPRS_REPLY + g);
	}
	GSM.print("AT+CIPSTART=\"UDP\",\"");
	sim900.printEEPROM(GPRS_HOST);
	GSM.print("\",\"");
	uint16_t portNumber = 0;
	EEPROM_readAnything(GPRS_PORT,portNumber);
	GSM.print(portNumber,DEC);
	GSM.println("\"");
	sim900.confirmAtCommand("T OK",2000);
	sim900.gsmSleepMode(2);
}


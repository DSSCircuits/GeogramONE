
uint8_t command0(uint8_t *cmd0)  //send coordinates
{
	uint8_t rssi = sim900.signalQuality(1);
	char atCommand[32];
	strcpy_P(atCommand,googlePrefix);
	if(sim900.sendMessage(0,smsData.smsNumber,NULL))
		return 1;
	GSM.print(lastValid.hour,DEC);
	GSM.print(":");
	GSM.print(lastValid.minute,DEC);
	GSM.print(":");
	GSM.print(lastValid.seconds,DEC);
	if(EEPROM.read(AMPM) & 0x01)
		GSM.print(lastValid.amPM[0]);
	GSM.print(",");
	GSM.print(lastValid.month,DEC);
	GSM.print("/");
	GSM.print(lastValid.day,DEC);
	GSM.print("/");
	GSM.println(lastValid.year,DEC);
	GSM.print("Ba=");GSM.print(max17043.getBatterySOC()/100,DEC);GSM.println(" %");
	GSM.print("RS=");GSM.println(rssi,DEC);
	#if USESATELLITESUSED
	GSM.print("Sa=");GSM.println(lastValid.satellitesUsed,DEC);
	#endif
	#if USEALTITUDE
	GSM.print("A=");GSM.println(lastValid.altitude,DEC);
	#endif
	#if USEPDOP
	GSM.print("P=");GSM.println(lastValid.pdop,DEC);
	#endif
	#if USEHDOP
	GSM.print("H=");GSM.println(lastValid.hdop,DEC);
	#endif
	#if USEVDOP
	GSM.print("V=");GSM.println(lastValid.vdop,DEC);
	#endif
	GSM.print("Ch = ");
	GSM.println(charge,HEX);
	GSM.print(atCommand);
	sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
	GSM.print("+(");
	#if USESPEEDKNOTS
	GSM.print(lastValid.speedKnots,DEC);
	#endif
	#if USECOURSE
	GSM.print(",");
	GSM.print(lastValid.courseDirection);
	#endif
	strcpy_P(atCommand,googleSuffix);
	GSM.println(atCommand);
	if(sim900.sendMessage(3,NULL,NULL))
		return 1;
	*cmd0 = 0;
	return 0;
}
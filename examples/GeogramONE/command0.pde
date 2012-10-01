
uint8_t command0()  //send coordinates
{
	uint16_t geoDataFormat;
	uint8_t rssi = sim900.signalQuality(1);
	uint8_t tFormat = EEPROM.read(TIMEFORMAT);
	EEPROM_readAnything(GEODATAFORMAT,geoDataFormat);
	char atCommand[32];
	strcpy_P(atCommand,googlePrefix);
	if(sim900.sendMessage(0,smsData.smsNumber,NULL))
		return 1;
	if(!geoDataFormat)
	{
		GSM.print(lastValid.hour,DEC);
		GSM.print(":");
		GSM.print(lastValid.minute,DEC);
		GSM.print(":");
		GSM.print(lastValid.seconds,DEC);
		if(tFormat & 0x01)
			GSM.print(lastValid.amPM[0]);
		GSM.print(",");
		if(!(tFormat & 0x02))
		{
			GSM.print(lastValid.month,DEC);
			GSM.print("/");
			GSM.print(lastValid.day,DEC);
			GSM.print("/");
			GSM.println(lastValid.year,DEC);
		}
		else
		{
			GSM.print(lastValid.year,DEC);
			GSM.print("/");
			GSM.print(lastValid.month,DEC);
			GSM.print("/");
			GSM.println(lastValid.day,DEC);
		}
		GSM.print("Ba=");GSM.print(max17043.getBatterySOC()/100,DEC);GSM.println(" %");
		GSM.print("RS=");GSM.println(rssi,DEC);
		#if USESATELLITESUSED
		GSM.print("Sa=");GSM.println(lastValid.satellitesUsed,DEC);
		#endif
		#if USEALTITUDE
		GSM.print("A=");GSM.println(lastValid.altitude,2);
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
	}
	else
	{
		if(geoDataFormat & 0x0001)
		{
			if(!(tFormat & 0x02))
			{
				GSM.print(lastValid.month,DEC);
				GSM.print("/");
				GSM.print(lastValid.day,DEC);
				GSM.print("/");
				GSM.println(lastValid.year,DEC);
			}
			else
			{
				GSM.print(lastValid.year,DEC);
				GSM.print("/");
				GSM.print(lastValid.month,DEC);
				GSM.print("/");
				GSM.println(lastValid.day,DEC);
			}
			GSM.print(",");
		}
		if(geoDataFormat & 0x0002)
		{
			GSM.print(lastValid.hour,DEC);
			GSM.print(":");
			GSM.print(lastValid.minute,DEC);
			GSM.print(":");
			GSM.print(lastValid.seconds,DEC);
			if(tFormat & 0x01)
				GSM.print(lastValid.amPM[0]);
			GSM.print(",");
		}
		if(geoDataFormat & 0x0004)
		{
			sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
			GSM.print(",");
		}
		if(geoDataFormat & 0x0008)
		{
			#if USESPEEDKNOTS
			GSM.print(lastValid.speedKnots,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0010)
		{
			#if USECOURSE
			GSM.print(lastValid.courseDirection);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0020)
		{
			#if USEALTITUDE
			GSM.print(lastValid.altitude,2);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0040)
		{
			#if USEHDOP
			GSM.print(lastValid.hdop,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0080)
		{
			#if USEVDOP
			GSM.print(lastValid.vdop,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0100)
		{
			#if USEPDOP
			GSM.print(lastValid.pdop,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0200)
		{
			#if USESATELLITESUSED
			GSM.print(lastValid.satellitesUsed,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0400)
		{
			#if USEMODE2
			GSM.print(lastValid.mode2,DEC);
			#endif
			GSM.print(",");
		}
		if(geoDataFormat & 0x0800)
		{
			GSM.print(max17043.getBatterySOC()/100,DEC);
			GSM.print(",");
		}
		if(geoDataFormat & 0x1000)
		{
			GSM.print(max17043.getBatteryVoltage()/1000.0,2);
			GSM.print(",");
		}
		if(geoDataFormat & 0x2000)
		{
			GSM.print(rssi,DEC);
			GSM.print(",");
		}
		if(geoDataFormat & 0x4000)
		{
			char eepChar;
			for (uint8_t ep = 0; ep < 25; ep++)
			{
				eepChar = EEPROM.read(ep + GEOGRAMONEID);
				if(eepChar == '\0')
					break;
				else
					GSM.print(eepChar);
			}
			GSM.println();
		}
	}
	if(sim900.sendMessage(3,NULL,NULL))
		return 1;
	cmd0 = 0;
	return 0;
}

void command0()  //send coordinates
{
	uint16_t geoDataFormat;
	uint8_t rssi = sim900.signalQuality(1);
	uint8_t tFormat = EEPROM.read(TIMEFORMAT);
	EEPROM_readAnything(GEODATAFORMAT2,geoDataFormat);
	if(sim900.sendMessage(0,smsData.smsNumber,NULL))
		return;
	if(!(geoDataFormat & 0x8000))
		printHTTP(&geoDataFormat, rssi);
	EEPROM_readAnything(GEODATAFORMAT1,geoDataFormat);
	printList(&geoDataFormat, rssi);
	GSM.println();
	if(!sim900.sendMessage(3,NULL,NULL))
		cmd0 = 0;
}

void printList(uint16_t *dataFormat, uint8_t rssi)
{
	uint8_t tFormat = EEPROM.read(TIMEFORMAT);
	if(*dataFormat & 0x0001)
	{
		if(!(tFormat & 0x02))
		{
			GSM.print(lastValid.month,DEC);
			GSM.print("/");
			GSM.print(lastValid.day,DEC);
			GSM.print("/");
			GSM.print(lastValid.year,DEC);
		}
		else
		{
			GSM.print(lastValid.year,DEC);
			GSM.print("/");
			GSM.print(lastValid.month,DEC);
			GSM.print("/");
			GSM.print(lastValid.day,DEC);
		}
		GSM.print(",");
	}
	if(*dataFormat & 0x0002)
	{
		GSM.print(lastValid.hour,DEC);
		GSM.print(":");
		GSM.print(lastValid.minute,DEC);
		GSM.print(":");
		GSM.print(lastValid.seconds,DEC);
		if(!(tFormat & 0x01))
			GSM.print(lastValid.amPM);
		GSM.print(",");
	}
	if(*dataFormat & 0x0004)
	{
		sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
		GSM.print(",");
	}
	if(*dataFormat & 0x0008)
	{
		GSM.print(lastValid.speed,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0010)
	{
		GSM.print(lastValid.courseDirection);
		GSM.print(",");
	}
	if(*dataFormat & 0x0020)
	{
		GSM.print(lastValid.altitude,2);
		GSM.print(",");
	}
	if(*dataFormat & 0x0040)
	{
		GSM.print(lastValid.hdop,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0080)
	{
		GSM.print(lastValid.vdop,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0100)
	{
		GSM.print(lastValid.pdop,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0200)
	{
		GSM.print(lastValid.satellitesUsed,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0400)
	{
		GSM.print(lastValid.mode2,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0800)
	{
		GSM.print(MAX17043getBatterySOC()/100,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x1000)
	{
		GSM.print(MAX17043getBatteryVoltage()*0.00125,2);
		GSM.print(",");
	}
	if(*dataFormat & 0x2000)
	{
		GSM.print(rssi,DEC);
		GSM.print(",");
	}
	if(*dataFormat& 0x4000)
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
	}
}


void printHTTP(uint16_t *dFormat, uint8_t rssi)
{
	uint16_t dataFormat = *dFormat & 0x7FFF;
	char eepChar;
	for (uint8_t ep = 0; ep < 100; ep++)
	{
		eepChar = EEPROM.read(ep + HTTP1);
		if(eepChar == '\0')
			break;
		else
			GSM.print(eepChar);
	}
	sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
	for (uint8_t ep = 0; ep < 100; ep++)
	{
		eepChar = EEPROM.read(ep + HTTP2);
		if(eepChar == '\0')
			break;
		else
			GSM.print(eepChar);
	}
	printList(&dataFormat, rssi);
	for (uint8_t ep = 0; ep < 100; ep++)
	{
		eepChar = EEPROM.read(ep + HTTP3);
		if(eepChar == '\0')
			break;
		else
			GSM.print(eepChar);
	}
	GSM.println();
}
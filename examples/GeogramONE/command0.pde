
void command0()  //send coordinates
{
	sim900.gsmSleepMode(0);
	uint16_t geoDataFormat;
	uint8_t rssi = sim900.signalQuality();
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
	sim900.gsmSleepMode(2);
}

void printList(uint16_t *dataFormat, uint8_t rssi)
{
	uint8_t tFormat = EEPROM.read(TIMEFORMAT);
	if(*dataFormat & 0x0001)
	{
		if(!(tFormat & 0x02))
		{
			GSM.print(lastValid.date[2]);
			GSM.print(lastValid.date[3]);
			GSM.print("/");
			GSM.print(lastValid.date[0]);
			GSM.print(lastValid.date[1]);
			GSM.print("/");
			GSM.print(lastValid.date + 4);
		}
		else
		{
			GSM.print(lastValid.date + 4);
			GSM.print("/");
			GSM.print(lastValid.date[2]);
			GSM.print(lastValid.date[3]);
			GSM.print("/");
			GSM.print(lastValid.date[0]);
			GSM.print(lastValid.date[1]);
		}
		GSM.print(",");
	}
	if(*dataFormat & 0x0002)
	{
		GSM.print(lastValid.time[0]);
		GSM.print(lastValid.time[1]);
		GSM.print(":");
		GSM.print(lastValid.time[2]);
		GSM.print(lastValid.time[3]);
		GSM.print(":");
		GSM.print(lastValid.time + 4);
		GSM.print(",");
	}
	if(*dataFormat & 0x0004)
	{
		printLatLon(&lastValid);
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
	//	#if USEHDOP
	//	GSM.print(lastValid.hdop,DEC);
	//	#endif
		GSM.print(",");
	}
	if(*dataFormat & 0x0080)
	{
	//	#if USEVDOP
	//	GSM.print(lastValid.vdop,DEC);
	//	#endif
		GSM.print(",");
	}
	if(*dataFormat & 0x0100)
	{
	//	#if USEPDOP
	//	GSM.print(lastValid.pdop,DEC);
	//	#endif
		GSM.print(",");
	}
	if(*dataFormat & 0x0200)
	{
		GSM.print(lastValid.satellitesUsed,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x0400)
	{
	//	#if USEMODE2
	//	GSM.print(lastValid.mode2,DEC);
	//	#endif
		GSM.print(",");
	}
	if(*dataFormat & 0x0800)
	{
		GSM.print(MAX17043getBatterySOC()/100,DEC);
		GSM.print(",");
	}
	if(*dataFormat & 0x1000)
	{
		GSM.print(MAX17043getBatteryVoltage()/1000.0,2);
		GSM.print(",");
	}
	if(*dataFormat & 0x2000)
	{
		GSM.print(rssi,DEC);
		GSM.print(",");
	}
	if(*dataFormat& 0x4000)
		sim900.printEEPROM(GEOGRAMONEID);
}


void printHTTP(uint16_t *dFormat, uint8_t rssi)
{
	uint16_t dataFormat = *dFormat & 0x7FFF;
	sim900.printEEPROM(HTTP1);
	printLatLon(&lastValid);
	sim900.printEEPROM(HTTP2);
	printList(&dataFormat, rssi);
	sim900.printEEPROM(HTTP3);
	GSM.println();
}

void printLatLon(goCoord *position)
{
	if(position->ns == 'S')
		GSM.print("-");
	GSM.print(position->latitude[0]);
	GSM.print(position->latitude[1]);
	GSM.print("+");
	GSM.print(position->latitude + 2);
	GSM.print(",");
	if(position->ew == 'W')
		GSM.print("-");
	GSM.print(position->longitude[0]);
	GSM.print(position->longitude[1]);
	GSM.print(position->longitude[2]);
	GSM.print("+");
	GSM.print(position->longitude + 3);
}
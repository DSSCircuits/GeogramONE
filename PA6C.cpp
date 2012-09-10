#include "PA6C.h"

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
    return i;
}


prog_char progmemStandbyGPS[] PROGMEM = "$PMTK161,0*28";  //13 characters

/*	CONSTRUCTOR	*/
PA6C::PA6C(HardwareSerial *ser)
{
	gpsSerial = ser;
}


uint8_t PA6C::init(unsigned long baudRate)
{
	gpsSerial->begin(baudRate);
}

uint8_t PA6C::saveCoordinates(gpsData *coord)
{
	#if USEMODE2
	if(coord->mode2 >=2 )
		return 0;
	else
		return 1;
	#else USEPOSITIONFIXIND
		return 0;
	#endif
}

/*************************************************************	
	Procedure to collect all data from used NMEA sentences. All
	data must be collected first for a valid position
	RETURN:
		0		All valid GPS data collected
		1		Continuing to collect data 
		2		Timed out while collecting data
		3		No data yet
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getTheData(gpsData *lastKnown)
{
	if(!gpsSerial->available())
		return 3;
	GPS gps;
	gpsData current;
	gps.dataCollected = 0x07;
	gps.returnStatus = 1;
	gps.timeOut = millis();
	while((gpsSerial->available()) || (millis() - gps.timeOut < GPSTIMEOUT))  //might want to change to OR instead of AND
	{
		if(gps.dataCollected & 0x01)
		{
			uint8_t senStat = getGPGGA(&gps, &current);
			if(!senStat)
			{
				gps.dataCollected &= ~(0x01);
				continue;
			}
			else if((senStat >= 1) && (senStat <= 17))
			{
				continue;
			}
			else if(senStat == 0xFF)
			{
				gps.dataCollected = 0x07;  // was originally 0x0F, changed for GSV
				return 0xFF;  //checksum error, all data invalid
			}
		}
		if(gps.dataCollected & 0x02)
		{
			uint8_t senStat = getGPGSA(&gps, &current);
			if(!senStat)
			{
				gps.dataCollected &= ~(0x02);
				continue;
			}
			else if((senStat >= 1) && (senStat <= 20))
			{
				continue;
			}
			else if(senStat == 0xFF)
			{
				gps.dataCollected = 0x07;
				return 0xFF;  //checksum error, all data invalid
			}
		}
		if(gps.dataCollected & 0x04)
		{
			uint8_t senStat = getGPRMC(&gps, &current);
			if(!senStat)
			{
				gps.dataCollected &= ~(0x04);
				break;
			}
			else if((senStat >= 1) && (senStat <= 15))
			{
				continue;
			}
			else if(senStat == 0xFF)
			{
				gps.dataCollected = 0x07;
				return 0xFF;  //checksum error, all data invalid
			}
		}
	}
	if(millis() - gps.timeOut > GPSTIMEOUT)
	{
		gps.dataCollected = 0x07; //was originally 0x0F
		return 2;
	}
	if(!(gps.dataCollected & 0x07)) //was originally 0x07
	{
		gps.dataCollected = 0x07; //reset to look for new data 
		if(!saveCoordinates(&current))
		{
			#if USECOURSE
			directionOfTravel(&current);
			#endif
			*lastKnown = current;
		}
		return 0; //new data available
	}
	return 1;
}

/*************************************************************	
	Procedure to sort and extract GPRMC sentence data
	RETURN:
		0		Valid GPRMC sentence data extracted
		1		Waiting for start of sentence
		2		Waiting to verify GPRMC sentence ID
		3-15	Collecting GPRMC data
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getGPRMC(GPS *gps, gpsData *currentPosition)
{
	while(gpsSerial->available())
	{
		switch(gps->returnStatus)
		{
			case 1 :
				lookForDollarSign(gps);
				continue;
				break;
			case 2 :
				getSentenceId(gps, GPRMC);
				continue;
				break;
			case 3 : 
				if(!nextField(gps))
				{
					#if NEWTIME
					currentPosition->utcTime = atol(gps->field);
					#else
					currentPosition->seconds = atol(gps->field)%100;
					currentPosition->minute = (atol(gps->field)%10000)/100;
					currentPosition->hour = (atol(gps->field)/10000)+TIMEZONE+24;
					if(currentPosition->hour == 24)
						currentPosition->hour = 0;
					if(currentPosition->hour > 24)
						currentPosition->hour -= 24;
					if(currentPosition->hour >= 12)
					{
						#if !AMPM
						if(currentPosition->hour > 12)
							currentPosition->hour -= 12;
						#endif
						currentPosition->amPM[0] = 'p';
					}
					else
						currentPosition->amPM[0] = 'a';
					#endif
				}
				continue;
				break;
			case 4 :
				if(!nextField(gps))
				{
					#if USERMCSTATUS
					currentPosition->rmcStatus = gps->field[0];
					#endif
				}
				continue;
				break;
			case 5 :
				if(!nextField(gps))
				{
					char *str = NULL;
					char *ptr = NULL;
					ptr = strtok_r(gps->field,".",&str);
					currentPosition->latitude = atol(ptr)*10000;
					ptr = strtok_r(NULL,"\0",&str);
					currentPosition->latitude += atol(ptr);
				}
				continue;
				break;
			case 6 :
				if(!nextField(gps))
				{
					if(gps->field[0] == 'S'){currentPosition->latitude = -currentPosition->latitude;}
				}
				continue;
				break;
			case 7 :
				if(!nextField(gps))
				{
					char *str = NULL;
					char *ptr = NULL;
					ptr = strtok_r(gps->field,".",&str);
					currentPosition->longitude = atol(ptr)*10000;
					ptr = strtok_r(NULL,"\0",&str);
					currentPosition->longitude += atol(ptr);
				}
				continue;
				break;
			case 8 :
				if(!nextField(gps))
				{
					if(gps->field[0] == 'W'){currentPosition->longitude = -currentPosition->longitude;}
				}
				continue;
				break;
			case 9 :
				if(!nextField(gps))
				{
					#if USESPEEDKNOTS
					float spK = atof(gps->field);
					if(MPHORKPH)
						spK *= KNOTSTOMPH;
					else
						spK *= KNOTSTOKPH;
					currentPosition->speedKnots = (uint16_t)spK;
					#endif
				}
				continue;
				break;
			case 10 :
				if(!nextField(gps))
				{
					#if USECOURSE
					currentPosition->course = (atof(gps->field)*100);
					#endif
				}
				continue;
				break;
			case 11 :
				if(!nextField(gps))
				{
					#if NEWTIME
					currentPosition->date = atol(gps->field);
					#else
					currentPosition->year = atol(gps->field)%100;
					currentPosition->month = (atol(gps->field)%10000)/100;
					currentPosition->day = atol(gps->field)/10000;
					#endif
				}
				continue;
				break;
			case 12 : case 13 :	case 14 :
				if(!nextField(gps))
				{
					#if USERMCSTATUS
					currentPosition->rmcStatus = gps->field[0];
					#endif
				}
				continue;
				break;
			case 15 : 
				if(!verifyChecksum(gps))
				{
					gps->returnStatus = 1;
					if(!gps->checksum){return 0;}//if checksum is 0 we pass
					else{return 0xFF;}
				}
				continue;
				break;
		}
	}
	return gps->returnStatus;
}
			
/*************************************************************	
	Procedure to sort and extract GPGGA sentence data
	RETURN:
		0		Valid GPGGA sentence data extracted
		1		Waiting for start of sentence
		2		Waiting to verify GPGGA sentence ID
		3-17	Collecting GPGGA data
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getGPGGA(GPS *gps, gpsData *currentPosition)
{
	while(gpsSerial->available())
	{
		switch(gps->returnStatus)
		{
			case 1 :
				lookForDollarSign(gps);
				continue;
				break;
			case 2 :
				getSentenceId(gps, GPGGA);
				continue;
				break;
			case 3 : case 4 : case 5 : 
			case 7 : case 8 :
				nextField(gps);
				continue;
				break;
			case 6 :
				if(!nextField(gps))
				{
					#if USEPOSITIONFIXIND
					currentPosition->positionFixInd = atoi(gps->field);
					#endif
				}
				continue;
				break;
			case 9 : 
				if(!nextField(gps))
				{
					#if USESATELLITESUSED
					currentPosition->satellitesUsed = atoi(gps->field);
					#endif
				}
				continue;
				break;
			case 10 :
				nextField(gps);
				continue;
				break;
			case 11 :
				if(!nextField(gps))
				{
					#if USEALTITUDE
					currentPosition->altitude = atof(gps->field);
					#endif
				}
				continue;
				break;
			case 12 : case 13 : case 14 : case 15 : case 16 :
				nextField(gps);  //we skip this field entry
				continue;
				break;
			case 17 : 
				if(!verifyChecksum(gps))
				{
					gps->returnStatus = 1;
					if(!gps->checksum){return 0;}//if checksum is 0 we pass
					else{return 0xFF;}
				}
				continue;
				break;
		}
	}
	return gps->returnStatus;
}

/*************************************************************	
	Procedure to sort and extract GPGSV sentence data
	RETURN:
		0		Valid GPGSV sentence data extracted
		1		Waiting for start of sentence
		2		Waiting to verify GPGSV sentence ID
		3-17	Collecting GPGSV data
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getGPGSV(GPS *gps, gpsData *currentPosition)
{
	while(gpsSerial->available())
	{
		switch(gps->returnStatus)
		{
			case 1 :
				lookForDollarSign(gps);
				continue;
				break;
			case 2 :
				getSentenceId(gps, GPGSV);
				continue;
				break;
			case 3 : case 4 :
				nextField(gps);  //we skip this field entry
				continue;
				break;
			 
			case 5 :
				if(!nextField(gps))
				{
					#if USESATINVIEW
					currentPosition->satInView = atoi(gps->field);
					#endif
				}
				continue;
				break;
			case 6 : case 7 : case 8 : 	case 9 : case 10 : case 11 :
			case 12 : case 13 : case 14 : case 15 : case 16 :
			case 17 : case 18 : case 19 : case 20 : case 21 :
				nextField(gps);  //we skip this field entry
				continue;
				break;
			case 22 : 
				if(!verifyChecksum(gps))
				{
					gps->returnStatus = 1;
					if(!gps->checksum){return 0;}//if checksum is 0 we pass
					else{return 0xFF;}
				}
				continue;
				break;
		}
	}
	return gps->returnStatus;
}



/*************************************************************	
	Procedure to sort and extract GPGSA sentence data
	RETURN:
		0		Valid GPGSA sentence data extracted
		1		Waiting for start of sentence
		2		Waiting to verify GPGSA sentence ID
		3-20	Collecting GPGSA data
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getGPGSA(GPS *gps, gpsData *currentPosition)
{
	while(gpsSerial->available())
	{
		switch(gps->returnStatus)
		{
			case 1 :
				lookForDollarSign(gps);
				continue;
				break;
			case 2 :
				getSentenceId(gps, GPGSA);
				continue;
				break;
			case 3 : 
				if(!nextField(gps))
				{
					#if USEMODE1
					currentPosition->mode1 = gps->field[0];
					#endif
				}
				continue;
				break;
			case 4 :
				if(!nextField(gps))
				{
					#if USEMODE2
					currentPosition->mode2 = atoi(gps->field);
					#endif
				}
				continue;
				break;
			case 5 : case 6 : case 7 : case 8 : case 9 :
			case 10 : case 11 : case 12 : case 13 : case 14 :
			case 15 : case 16 :
				nextField(gps);
				continue;
				break;
			case 17 :
				if(!nextField(gps))
				{
					#if USEPDOP 
					currentPosition->pdop = (uint16_t)(atof(gps->field)*100);
					#endif
				}
				continue;
				break;
			case 18 :
				if(!nextField(gps))
				{
					#if USEHDOP
					currentPosition->hdop = (uint16_t)(atof(gps->field)*100);
					#endif
				}
				continue;
				break;
			case 19 :
				if(!nextField(gps))
				{
					#if USEVDOP
					currentPosition->vdop = (uint16_t)(atof(gps->field)*100);
					#endif
				}
				continue;
				break;
			case 20 : 
				if(!verifyChecksum(gps))
				{
					gps->returnStatus = 1;
					if(!gps->checksum){return 0;}//if checksum is 0 we pass
					else{return 0xFF;}
				}
				continue;
				break;
		}
	}
	return gps->returnStatus;
}

/*************************************************************	
	Procedure to sort and extract PMTK001 sentence data
	RETURN:
		0		Valid PMTK001 sentence data extracted
		1		Waiting for start of sentence
		2		Waiting to verify PMTK001 sentence ID
		3-5		Collecting PMTK001 data
		0xFF	Checksum failure, data invalid
**************************************************************/
uint8_t PA6C::getPMTK001(GPS *gps)
{
	while(gpsSerial->available())
	{
		switch(gps->returnStatus)
		{
			case 1 :
				lookForDollarSign(gps);
				continue;
				break;
			case 2 :
				getSentenceId(gps, PMTK001);
				continue;
				break;
			case 3 :
				if(!nextField(gps))
				{
					pmtk001.p001Cmd = atoi(gps->field);
				}
				continue;
				break;
			case 4 :
				if(!nextField(gps))
				{
					pmtk001.p001Flag = atoi(gps->field);
				}
				continue;
				break;
			case 5 : 
				if(!verifyChecksum(gps))
				{
					gps->returnStatus = 1;
					if(!gps->checksum){return 0;}//if checksum is 0 we pass
					else{return 0xFF;}
				}
				continue;
				break;
		}
	}
	return gps->returnStatus;
}



void PA6C::lookForDollarSign(GPS *gpsTemp)
{
	if(gpsSerial->read() != '$'){return;} //waiting for $
	gpsTemp->checksum = 0;
	gpsTemp->index = 0;
	gpsTemp->field[gpsTemp->index] = '\0';
	gpsTemp->returnStatus = 2;
}

void PA6C::getSentenceId(GPS *gpsTemp, char *sId)
{
	gpsTemp->field[gpsTemp->index] = gpsSerial->read();
	gpsTemp->checksum ^= gpsTemp->field[gpsTemp->index];
	if(gpsTemp->field[gpsTemp->index] != ',')
	{
		gpsTemp->index++;
		gpsTemp->field[gpsTemp->index] = '\0';
		return;
	}
	if(strstr(gpsTemp->field,sId)!=NULL)
	{
		gpsTemp->returnStatus = 3;
	}
	else
	{
		gpsTemp->returnStatus = 1;
	}
	gpsTemp->index = 0;
	gpsTemp->field[gpsTemp->index] = '\0';
}

uint8_t PA6C::nextField(GPS *gpsTemp)
{
	gpsTemp->field[gpsTemp->index] = gpsSerial->read();
	gpsTemp->checksum ^= gpsTemp->field[gpsTemp->index];
	if((gpsTemp->field[gpsTemp->index] == ',') || (gpsTemp->field[gpsTemp->index] == '*'))
	{
		gpsTemp->field[gpsTemp->index] = '\0';
		gpsTemp->index = 0;
		gpsTemp->returnStatus++;
		return 0;
	}
	else
	{
		gpsTemp->index++;
		return 1;
	}
}

uint8_t PA6C::verifyChecksum(GPS *gpsTemp)
{
	gpsTemp->field[gpsTemp->index] = gpsSerial->read();
	if((gpsTemp->field[gpsTemp->index] == '\r') || (gpsTemp->index >= 2))
	{
		uint8_t checksumReference = 0;
		gpsTemp->checksum ^= '*';
		if(((gpsTemp->field[0]) >= 48) && ((gpsTemp->field[0]) <= 57))
		{
			checksumReference = (gpsTemp->field[0]-48) * 16;
		}
		else if ((gpsTemp->field[0] >= 'A') && (gpsTemp->field[0] <= 'F'))
		{
			checksumReference = (gpsTemp->field[0] - 'A' + 10 ) * 16;
		}
		if(((gpsTemp->field[1]) >= 48) && ((gpsTemp->field[1]) <= 57))
		{
			checksumReference |= (gpsTemp->field[1]-48);
		}
		else if ((gpsTemp->field[1] >= 'A') && (gpsTemp->field[1] <= 'F'))
		{
			checksumReference |= (gpsTemp->field[1] - 'A' + 10 );
		}
		gpsTemp->checksum ^= checksumReference;
		return 0;
	}
	gpsTemp->index++;
	return 1;
}

uint8_t PA6C::sleepGPS()
{
	char atCommand[20];
	strcpy_P(atCommand,progmemStandbyGPS);
	gpsSerial->println(atCommand);
	delay(300);
	gpsSerial->flush();
	unsigned long timeOut = millis();
	while(millis() - timeOut <= GPSTIMEOUT)
	{
		if(gpsSerial->available() <= 30);
		//if(getTheData() == 2)
		{
		  return(0);
		}
	}
    return(1);
}

uint8_t PA6C::wakeUpGPS()
{
  gpsSerial->println("$PMTK000*32");
  unsigned long timeOut = millis();
  while (millis() - timeOut <= GPSTIMEOUT) // was 3000 before
  {
    if(gpsSerial->available() > 50){return(0);} //was originally 5, changed to 50 to let it wake up
  }
  return(1);
}

uint8_t PA6C::geoFenceDistance(gpsData *last, geoFence *fence)  // was originally longs and called fLat and fLon
{
	float ToRad = PI / 180.0;
	float R = 6378.1;   // radius earth in Km
	float dLat = (fence->latitude - (((last->latitude%1000000)/600000.0) + (last->latitude/1000000))) * ToRad;
	float dLon = (fence->longitude - (((last->longitude%1000000)/600000.0) + (last->longitude/1000000))) * ToRad;
	float a = sin(dLat/2) * sin(dLat/2) +
		cos((((last->latitude%1000000)/600000.0) + (last->latitude/1000000)) * ToRad) * cos(fence->latitude * ToRad) * 
		sin(dLon/2) * sin(dLon/2); 
		
	float c = 2 * atan2(sqrt(a), sqrt(1 - a)); 
	unsigned long d = (unsigned long)(R * c * 1000UL);
	//float d = (R * c);
	#if MPHORKPH
		d *= METERSTOFEET;
	#endif
	if(!fence->inOut) //inside fence
	{
		if( d > fence->radius )
		//if(((unsigned long)(d * 1000UL)) > fence->radius )
			return 0;
		else
			return 1;
	}
	if(fence->inOut) //outside fence
	{
		if( d < fence->radius )
		//if(((unsigned long)(d * 1000UL)) < fence->radius )
			return 0;
		else
			return 1;
	}
}

uint8_t PA6C::configureFence(uint8_t fenceNumber, geoFence *fence)
{
	uint8_t offset = 0;
	if(!fenceNumber || (fenceNumber > 3) )
	{
		return 1;
	}
	if(fenceNumber == 2)
		offset += 15;
	if(fenceNumber == 3)
		offset += 30;
	EEPROM_readAnything((INOUT1 + offset), fence->inOut);
	EEPROM_readAnything((RADIUS1 + offset), fence->radius);
	EEPROM_readAnything((LATITUDE1 + offset), fence->latitude);
	EEPROM_readAnything((LONGITUDE1 + offset), fence->longitude);
	return 0;
}

uint8_t PA6C::isFenceActive(uint8_t fenceNumber, uint8_t *fenceVar)
{
	uint8_t offset = 0;
	if(!fenceNumber || (fenceNumber > 3) )
	{
		*fenceVar = 0;
		return 1;
	}
	if(fenceNumber == 2)
		offset += 15;
	if(fenceNumber == 3)
		offset += 30;
	*fenceVar = EEPROM.read(ACTIVE1 + offset);
	return 0;
}
	
		

#if USECOURSE
void PA6C::directionOfTravel(gpsData *current)
{
  if (current->course <= 2300 || current->course > 33800)
    strcpy(current->courseDirection,"N");
  if (current->course > 2300 && current->course <= 6800)
    strcpy(current->courseDirection,"NE");
  if (current->course > 6800 && current->course <= 11300)
    strcpy(current->courseDirection,"E");
  if (current->course > 11300 && current->course <= 15800)
    strcpy(current->courseDirection,"SE");
  if (current->course > 15800 && current->course <= 20300)
    strcpy(current->courseDirection,"S");
  if (current->course > 20300 && current->course <= 24800)
    strcpy(current->courseDirection,"SW");
  if (current->course > 24800 && current->course <= 29300)
    strcpy(current->courseDirection,"W");
  if (current->course > 29300 && current->course <= 33800)
    strcpy(current->courseDirection,"NW");
}  
#endif

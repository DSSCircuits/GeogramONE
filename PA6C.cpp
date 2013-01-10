#include "PA6C.h"


/*	CONSTRUCTOR	*/
PA6C::PA6C(HardwareSerial *ser)
{
	gpsSerial = ser;
}

uint8_t PA6C::init(unsigned long baudRate)
{
	gpsSerial->begin(baudRate);
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
uint8_t PA6C::getTheData2(gpsData *lastKnown)
{
	uint8_t returnStatus = 0x01;
	static bool startNew = false; 
	static char gpsField[15];
	static uint8_t charIndex = 0;
	static uint8_t checksum = 0;
	static uint8_t checksumR = 0;
	static uint8_t sentenceIndex = 0x07; //we're only collection 3 sentences
	static uint8_t sentenceID = 0;
	static uint8_t fieldID = 0;
	static gpsData currentPosition;
	if(!gpsSerial->available()) //no data in the buffer, safe to return
		return 3;
	while(gpsSerial->available()) 
	{
		gpsField[charIndex] = gpsSerial->read();
		gpsField[charIndex+1] = '\0';
		if((gpsField[charIndex] != '$') && (!startNew)) 
			continue;
		switch(gpsField[charIndex])
		{
			case '$':
				charIndex = 0;
				checksum = 0;
				startNew = true;
				break;
			case ',':
				checksum ^= gpsField[charIndex];
				gpsField[charIndex] = '\0';
				charIndex = 0;
				filterData(gpsField, &currentPosition, &sentenceID, &fieldID); //filter the data
				break; 
			case '*':
				checksumR = checksum;
				checksum = 0;
				charIndex = 0;
				filterData(gpsField, &currentPosition, &sentenceID, &fieldID); //filter the data
				break;
			case '\n':
				break;
			case '\r':
				checksum = 0;
				if((gpsField[0]) >= 48 && (gpsField[0]) <= 57)
					checksum = (gpsField[0]-48) << 4;
				else
					checksum = (gpsField[0]-55) << 4;
				if((gpsField[1]) >= 48 && (gpsField[1]) <= 57)
					checksum |= (gpsField[1]-48);
				else
					checksum |= (gpsField[1]-55);	
				if(checksumR != checksum)
					sentenceIndex = 0x00; //failed checksum no data valid, start over
				if(sentenceIndex = 0x07)
					*lastKnown = currentPosition;
				startNew = false;	
				checksumR = 0;
				checksum = 0;
				charIndex = 0;
				fieldID = 0;
				break;
			default:
				checksum ^= gpsField[charIndex];
				charIndex++;
				break;
		}
	}
}

void PA6C::filterData(char *fieldData, gpsData *current, uint8_t *sID, uint8_t *fID)
{
	if(!(*fID))
	{
		if(strstr(fieldData,"GGA")!=NULL)
		{		
			*sID = 4;
			*fID++;
			return;
		}
		if(strstr(fieldData,"GSA")!=NULL)
		{		
			*sID |= 2;
			*fID++;
			return;
		}
		if(strstr(fieldData,"GSV")!=NULL)
			return;
		if(strstr(fieldData,"RMC")!=NULL)
		{		
			*sID |= 1;
			if(*sID != 0x07)
			{
				*sID = 0;
				*fID = 0;
			}
			*fID++;
			return;
		}
		if(strstr(fieldData,"VTG")!=NULL)
			return;
		*sID = 0;
		*fID = 0;
	}
	if(!(*sID))
		*fID = 0;
	if(*sID == 4)
	{
		switch(*fID)
		{
			case 6:
				current->positionFixInd = atoi(fieldData);
				break;
			case 7:	
				current->satellitesUsed = atoi(fieldData);
				break;
			case 9:
				current->altitude = atof(fieldData);
				break;
			case 14:
				*sID = 2;
				*fID = 0;
				return;
				break;
		}
		*fID++;
	}
	if(*sID == 2)
	{
		switch(*fID)
		{
			case 1:
				current->mode1 = fieldData[0];
				break;
			case 2:	
				current->mode2 = atoi(fieldData);
				break;
			case 15:
				current->pdop = (uint16_t)(atof(fieldData)*100);
				break;
			case 16:	
				current->hdop = (uint16_t)(atof(fieldData)*100);
				break;
			case 17:
				current->vdop = (uint16_t)(atof(fieldData)*100);
				*sID = 1;
				*fID = 0;
				return;
				break;
		}
		*fID++;
	}	
	if(*sID == 1)
	{
		switch(*fID)
		{
			case 1:
				current->seconds = atol(fieldData)%100;
				current->minute = (atol(fieldData)%10000)/100;
				current->hour = (atol(fieldData)/10000);
				break;
			case 2:
				current->rmcStatus = fieldData[0];
				break;
			case 3:	
				{
					char *str = NULL;
					char *ptr = NULL;
					ptr = strtok_r(fieldData,".",&str);
					current->latitude = atol(ptr)*10000;
					ptr = strtok_r(NULL,"\0",&str);
					current->latitude += atol(ptr);
				}
				break;
			case 4:	
				if(fieldData[0] == 'S'){current->latitude = -current->latitude;}
				break;
			case 5:
				{
					char *str = NULL;
					char *ptr = NULL;
					ptr = strtok_r(fieldData,".",&str);
					current->longitude = atol(ptr)*10000;
					ptr = strtok_r(NULL,"\0",&str);
					current->longitude += atol(ptr);
				}
				break;
			case 6:	
				if(fieldData[0] == 'W'){current->longitude = -current->longitude;}
				break;
			case 7:	
				current->speedKnots = (uint16_t)(atof(fieldData) * 100);
				break;
			case 8:	
				current->course = (uint16_t)(atof(fieldData)*100);
				break;
			case 9:	
				{
					current->year = atol(fieldData)%100;
					current->month = (atol(fieldData)%10000)/100;
					current->day = atol(fieldData)/10000;
				}
				break;
			case 12:
				*sID = 7;
				*fID = 0;
				return;
				break;
				
		}
		*fID++;
	}	
}

uint8_t PA6C::sleepGPS()
{
	gpsSerial->println("$PMTK161,0*28");
	delay(300);
	gpsSerial->flush();
	unsigned long timeOut = millis();
	while(millis() - timeOut <= GPSTIMEOUT)
	{
		if(gpsSerial->available() <= 30);
			return 0;
	}
    return 1;
}

uint8_t PA6C::wakeUpGPS()
{
	gpsSerial->println("$PMTK000*32");
	unsigned long timeOut = millis();
	while((millis() - timeOut) <= GPSTIMEOUT) // was 3000 before
	{
		if(gpsSerial->available() > 50)
			return 0; //was originally 5, changed to 50 to let it wake up
	}
	return 1;
}


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

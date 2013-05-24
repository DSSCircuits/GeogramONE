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
		2		No data in the buffer
**************************************************************/
uint8_t PA6C::getCoordinates(goCoord *lastKnown)
{
	/*static*/ bool startNew = false; 
	/*static*/ char gpsField[15];
	/*static*/ uint8_t charIndex = 0;
	/*static*/ uint8_t checksum = 0;
	/*static*/ uint8_t checksumR = 0;
	/*static*/ uint8_t sentenceID = 0;
	/*static*/ uint8_t fieldID = 0;
	/*static*/ goCoord currentPosition;
	if(!gpsSerial->available()) //no data in the buffer, safe to return
		return 2;
	unsigned long getGpsData = millis();
	while((millis() - getGpsData) < 45 )
	{
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
			case '\r': // used to be \n
				break;
			case '\n': // used to be \r
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
					sentenceID = 0x00;
				startNew = false;	
				checksumR = 0;
				checksum = 0;
				charIndex = 0;
				fieldID = 0;
				/*if(sentenceID == 0x07)*/if(sentenceID == 0x0F)
				{
				/*	sentenceID = 0x00;*/
					if(currentPosition.signalLock)
					{
						directionOfTravel(&currentPosition);
						currentPosition.updated = 0x01;
						*lastKnown = currentPosition;
					}
					else
						lastKnown->signalLock = false;
					return 0;
				}
				break;
			default:
				checksum ^= gpsField[charIndex];
				charIndex++;
				break;
		}
	}
	}
	return 1; //all data in the buffer processed, but new data is not available
}

void PA6C::filterData(char *fieldData, goCoord *current, uint8_t *sID, uint8_t *fID)
{
	if(!(*fID))
	{
		if(strstr(fieldData,GPGGA)!=NULL)
		{		
			*sID = 4;
			(*fID)++;
			return;
		}
		if(strstr(fieldData,GPGSA)!=NULL)
		{		
			*sID |= 2;
			(*fID)++;
			return;
		}
		if(strstr(fieldData,GPGSV)!=NULL)
			return;
		if(strstr(fieldData,GPRMC)!=NULL)
		{		
			*sID |= 1;
		/*	if(*sID != 0x07)
			{
				*sID = 0;
				*fID = 0;
			}*/
			(*fID)++;
			return;
		}
		if(strstr(fieldData,GPVTG)!=NULL)
		{		
			*sID |= 0x08;
		/*	if(*sID != 0x0F)
			{
				*sID = 0;
				*fID = 0;
			}*/
			(*fID)++;
			return;
		}
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
				current->altitude = atol(fieldData);
				break;
			case 14:
				*fID = 0;
				return;
				break;
		}
		(*fID)++;
	}
	if(*sID == 6)
	{
		switch(*fID)
		{
			case 1:
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
				*fID = 0;
				return;
				break;
		}
		(*fID)++;
	}	
	if(*sID == 7)
	{
		switch(*fID)
		{
			case 1:
				fieldData[6] = '\0';
				strcpy(current->time,fieldData);
				break;
			case 2:
				if(fieldData[0] == 'A')
					current->signalLock = true;
				else
					current->signalLock = false;
				break;
			case 3:	
				strcpy(current->latitude,fieldData);
				break;
			case 4:
				current->ns = fieldData[0];
				break;
			case 5:
				strcpy(current->longitude,fieldData);
				break;
			case 6:	
				current->ew = fieldData[0];
				break;
			case 7:
				current->speed = atol(fieldData);
				break;
			case 8:
				current->course = atoi(fieldData);
				break;
			case 9:
				strcpy(current->date,fieldData);
				break;
			case 12:
				*fID = 0;
				return;
				break;
		}
		(*fID)++;
	}
	if(*sID == 0x0F)
	{
		switch(*fID)
		{
			case 9:
				*fID = 0;
				return;
				break;
		}
		(*fID)++;
	}
}

uint8_t PA6C::sleepGPS()
{
	gpsSerial->println(PMTK161);
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
	gpsSerial->println(PMTK000);
	unsigned long timeOut = millis();
	while((millis() - timeOut) <= GPSTIMEOUT) // was 3000 before
	{
		if(gpsSerial->available() > 50)
			return 0; //was originally 5, changed to 50 to let it wake up
	}
	return 1;
}


void PA6C::directionOfTravel(goCoord *current)
{
  if (current->course <= 23 || current->course > 338)
    strcpy(current->courseDirection,"N");
  if (current->course > 23 && current->course <= 68)
    strcpy(current->courseDirection,"NE");
  if (current->course > 68 && current->course <= 113)
    strcpy(current->courseDirection,"E");
  if (current->course > 113 && current->course <= 158)
    strcpy(current->courseDirection,"SE");
  if (current->course > 158 && current->course <= 203)
    strcpy(current->courseDirection,"S");
  if (current->course > 203 && current->course <= 248)
    strcpy(current->courseDirection,"SW");
  if (current->course > 248 && current->course <= 293)
    strcpy(current->courseDirection,"W");
  if (current->course > 293 && current->course <= 338)
    strcpy(current->courseDirection,"NW");
}  

uint8_t PA6C::geoFenceDistance(goCoord *last, geoFence *fence) // was originally longs and called fLat and fLon
{
	float ToRad = PI / 180.0;
	float R = 6378.1; // radius earth in Km
	float lLat = ((uint16_t)(atoi(last->latitude)/100)) + (atof(last->latitude + 2) / 60.0);
	float lLon = ((uint16_t)(atoi(last->longitude)/100)) + (atof(last->longitude + 2) / 60.0);
	if(last->ns == 'S')
		lLat *= -1.0;
	if(last->ew == 'W')
		lLon *= -1.0;
	float dLat = ((((fence->latitude%1000000)/600000.0) + (fence->latitude/1000000)) - (lLat)) * ToRad;
	float dLon = ((((fence->longitude%1000000)/600000.0) + (fence->longitude/1000000)) - (lLon)) * ToRad;
	float a = sin(dLat/2) * sin(dLat/2) + cos((lLat) * ToRad) * cos((lLat) * ToRad) * sin(dLon/2) * sin(dLon/2);
	float c = 2 * atan2(sqrt(a), sqrt(1 - a));
	unsigned long d = (unsigned long)(R * c * 1000UL);
//	if(!impMetric)  //might need to revisit this line
		d *= METERSTOFEET;
	if(!fence->inOut) //inside fence
	{
		if( d > fence->radius )
			return 0;
		else
			return 1;
	}
	if(fence->inOut) //outside fence
	{
		if( d < fence->radius )
			return 0;
		else
			return 1;
	}
}

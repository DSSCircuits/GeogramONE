uint8_t initUDP()
{
	sim900.gsmSleepMode0();
	if(!sendGPRScmd ("AT+CGATT?","OK",3000,true,0))
		return 1;
	if(!sendGPRScmd ("AT+CIPSHUT","OK",3000,true,0))
		return 2;
	if(!sendGPRScmd ("AT+CSTT=\"wholesale\"","OK",3000,true,0))
	//if(!sendGPRScmd ("AT+CSTT=\"telnamobile.com\"","OK",3000,true,0))
	//if(!sendGPRScmd("AT+CSTT=\"epc.tmobile.com\"","OK",10000,true,0))
		return 3;
	if(!sendGPRScmd ("AT+CIICR","OK",10000,true,0))
		return 4;
	if(!sendGPRScmd ("AT+CIFSR",".",5000,true,0))
		return 5;
	if(!sendGPRScmd ("AT+CIPSTART=\"UDP\",\"193.193.165.166\",\"20332\"","CONNECT OK",2000,true,0))
		return 6;
	if(!sendGPRScmd ("AT+CIPSTATUS","PDP DEACT",3000,true,0))
		return 7;
	udp &= ~(0x02);
	return 0;
	
}


/***** New Send AT command to cover everything ********
Requirements
	- send AT Command
	- send additional AT Command parameters
	- send 0x1A to send byte
	- send 0x1B to cancel send
	- send string from EEPROM address
	
	- check OK      		bit 0
	- check ERROR   		bit 1
	- check passed string 	bit 2
	
	- number of retries
	
	- option for flush() on entrance  bit 0
	- option for flush() on exit      bit 1
	- option to use print or println  bit 2
	
uint8_t sendAtCommand(const char* atCommand, const char* response, uint8_t checks, uint8_t options, uint8_t retries, unsigned long timeout, boolean useArg, const char* argument)
{
	uint8_t index = 0;
	char rxBuffer[22];
	unsigned long tOut;
	for(uint8_t nr = 0; nr < retries; nr++)
	{
		if(options & 0x01)
			GSM.flush();
		GSM.print(atCommand);
		if(useArg)
			GSM.print(argument);
		if(options & 0x04)
			GSM.println();
		timeout = millis();
		index = 0;
		while (millis() - tOut <= timeout)
		{
			if(GSM.available())
			{
				rxBuffer[index] = GSM.read();
				index++;
				rxBuffer[index] = '\0';
				if((strstr(rxBuffer,"OK") != NULL) && (checks & 0x01))
				{
					if(options & 0x02)
						GSM.flush();
					return 0;
				}
				if((strstr(rxBuffer,"ERROR") != NULL) && (checks & 0x02))
				{
					if(options & 0x02)
						GSM.flush();
					return 1;
				}
				if((strstr(rxBuffer,response) != NULL) && (checks & 0x04))
				{
					if(options & 0x02)
						GSM.flush();
					return 0;
				}
				if((rxBuffer[index-1] = '\n') || (index == 21))  //keeps buffer size low
					index = 0;
			}
		}
	}
	if(options & 0x02)
		GSM.flush();
	return 2;
}



******************************************************/


uint8_t sendAtCommand(const char* atCommand, const char* response, uint8_t checks, uint8_t options, uint8_t retries, unsigned long timeout, boolean useArg, const char* argument)
{
	uint8_t index = 0;
	char rxBuffer[22];
	unsigned long tOut;
	for(uint8_t nr = 0; nr < retries; nr++)
	{
		if(options & 0x01)
			GSM.flush();
		GSM.print(atCommand);
		if(useArg)
			GSM.print(argument);
		if(options & 0x04)
			GSM.println();
		timeout = millis();
		index = 0;
		while (millis() - tOut <= timeout)
		{
			if(GSM.available())
			{
				rxBuffer[index] = GSM.read();
				index++;
				rxBuffer[index] = '\0';
				if((strstr(rxBuffer,"OK") != NULL) && (checks & 0x01))
				{
					if(options & 0x02)
						GSM.flush();
					return 0;
				}
				if((strstr(rxBuffer,"ERROR") != NULL) && (checks & 0x02))
				{
					if(options & 0x02)
						GSM.flush();
					return 1;
				}
				if((strstr(rxBuffer,response) != NULL) && (checks & 0x04))
				{
					if(options & 0x02)
						GSM.flush();
					return 0;
				}
				if((rxBuffer[index-1] = '\n') || (index == 21))  //keeps buffer size low
					index = 0;
			}
		}
	}
	if(options & 0x02)
		GSM.flush();
	return 2;
}


void UDP()
{
	if(udp & 0x02)
		if(initUDP())
			return;
	sim900.gsmSleepMode0();
	if(!sendAtCommand("AT+CGATT?",": 1",0x04,0x07,0,2000,false,NULL))
//	if(!sendGPRScmd ("AT+CGATT?","OK",3000,true,0))
		return;
/*	if(sendGPRScmd ("AT+CIPSTATUS","PDP DEACT",100,true,0))
	{
		sendGPRScmd ("AT+CIPSHUT","OK",1000,true,0);
		return;
	}*/
/*	sendGPRScmd ("AT+CSTT=\"wholesale\"","OK",3000,true,0);
	//sendGPRScmd ("AT+CSTT=\"telnamobile.com\"","OK",3000,true,0);
	//sendGPRScmd("AT+CSTT=\"epc.tmobile.com\"","OK",10000,true,0);
	sendGPRScmd ("AT+CIICR","OK",10000,true,0);*/
	
	if(!sendAtCommand("AT+CIFSR",".",0x04,0x07,0,5000,false,NULL))
//	if(!sendGPRScmd ("AT+CIFSR",".",5000,true,0))
	{
		udp |= 0x02;
		return;
	}
//	sendGPRScmd ("AT+CIPSTART=\"UDP\",\"193.193.165.166\",\"20332\"","CONNECT OK",2000,true,0);
	if(!sendAtCommand("AT+CIPSEND",">",0x04,0x07,0,3000,false,NULL))
//	if(!sendGPRScmd ("AT+CIPSEND",">",3000,true,0))
	{
		udp |= 0x02;
		return;
	}
	GSM.print("012896006334665#SD#");
//	GSM.print(lastValid.orangeDate);
	GSM.print("NA");
	GSM.print(";");
//	GSM.print(lastValid.orangeTime);
	GSM.print("NA");
	GSM.print(";");
	GSM.print(lastValid.orangeLat,4);
	GSM.print(";");
	GSM.print(lastValid.orangeNS);
	GSM.print(";");
	GSM.print("0");
	GSM.print(lastValid.orangeLon,4);
	GSM.print(";");
	GSM.print(lastValid.orangeEW);
	GSM.print(";");
	GSM.print(lastValid.orangeSpeed);
	GSM.print(";");
	GSM.print(lastValid.orangeCourse);
	GSM.print(";");
	GSM.print(lastValid.orangeAltitude);
	GSM.print(";");
	GSM.println("NA");
	GSM.println(0x1A,BYTE);
	if(!sendGPRScmd ("","SEND OK",3000,false,0))
	{
		udp |= 0x02;
		return;
	}
    sendGPRScmd ("","#ASD#1",1000,false,0);
	udp = 0;
	
	sendAtCommand("AT+CIPCLOSE",NULL,0x01,0x07,0,3000,false,NULL);
//	sendGPRScmd ("AT+CIPCLOSE","OK",2000,true,0);
//	sendGPRScmd ("AT+CIPSHUT","OK",2000,true,0);
}

void udpSetup()
{
        
	/*sim900.gsmSleepMode0();
	sendGPRScmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",20000,true,0);
	sendGPRScmd("AT+SAPBR=3,1,\"APN\",\"epc.tmobile.com\"","OK",20000,true,0);
	sendGPRScmd("AT+SAPBR=1,1","OK",20000,true,0);// Tries to connect GPRS 
	sendGPRScmd("AT+HTTPINIT","OK",10000,true,0);
	sendGPRScmd("AT+HTTPPARA=\"URL\",\"www.google.com\"","OK",20000,true,0);
	delay(1000);
	sendGPRScmd("AT+HTTPACTION=0","OK",10000,true,0);
	delay(10000);
	sendGPRScmd("AT+HTTPREAD","OK",10000,true,0);
	sendGPRScmd("AT+SAPBR=0,1","OK",20000,true,0);// Disconnect GPRS
	return;*/
	
	sim900.gsmSleepMode0();
	sendGPRScmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",20000,true,0);
	sendGPRScmd("AT+SAPBR=3,1,\"APN\",\"epc.tmobile.com\"","OK",20000,true,0);
	sendGPRScmd("AT+SAPBR=1,1","OK",20000,true,0);// Tries to connect GPRS 
	sendGPRScmd("AT+HTTPINIT","OK",10000,true,0);
	sendGPRScmd("AT+HTTPPARA=\"CID\",1","OK",10000,true,0);
	sendGPRScmd("AT+HTTPPARA=\"URL\",\"posttestserver.com/post.php\"","OK",20000,true,0);
//	delay(1000);
        sendGPRScmd("AT+HTTPDATA=10,10000","DOWN",10000,true,0);
        GSM.println("GeogramONE");
        delay(500); //5000 good
	sendGPRScmd("AT+HTTPACTION=1","ACTION",10000,true,0);
        delay(5000);
	sendGPRScmd("AT+HTTPTERM","OK",10000,true,0);
	sendGPRScmd("AT+SAPBR=0,1","OK",20000,true,0);// Disconnect GPRS
	return;
	
	
	
	
	
	sendGPRScmd("AT+CIPMUX=0","OK",10000,true,0);
//	sendGPRScmd("AT+CGATT=1","OK",20000,true,0);// GPRS Status
	sendGPRScmd("AT+CSTT=\"epc.tmobile.com\"","OK",10000,true,0);
	sendGPRScmd("AT+CIICR","OK",20000,true,0); 
	delay(1000);
	sendGPRScmd("AT+CIFSR",".",20000,true,0);  
	delay(10000);
	sendGPRScmd("AT+CIPSTART=\"UDP\",\"online.gpsgate.com\",\"30175\"","T OK",20000,true,0);
//	sendGPRScmd("AT+CIPSTART=\"UDP\",\"172.30.0.212\",\"5000\"","T OK",20000,true,0);
	sendGPRScmd("AT+CIPSEND",">",30000,true,0);
	GSM.println("$FRLIN,IMEI,012896006334665,*48");
	GSM.println("$GPRMC,154403.000,A,4222.8838,N,07133.3933,W,0.000,0.0,180113,,*1D");
	
	
//	GSM.println("test");
	GSM.print(0x1A,BYTE);//send the data to the server
    delay(5000);
	sendGPRScmd("AT+CIPCLOSE","OK",20000,true,0);
	sendGPRScmd("AT+CIPSHUT","OK",20000,true,0);
//	sendGPRScmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",20000,true,0);
//	sendGPRScmd("AT+SAPBR=3,1,\"APN\",\"epc.tmobile.com\"","OK",20000,true,0);
//	sendGPRScmd("AT+SAPBR=1,1","OK",20000,true,0);// Tries to connect GPRS 
//	sendGPRScmd("AT+SAPBR=2,1","OK",20000,true,0);// Checks Status of GPRS 
//	delay(10000);
//	sendGPRScmd("AT+SAPBR=0,1","OK",20000,true,0);// Disconnect GPRS



/*	sendGPRScmd("AT+CIPMUX=0","OK",10000,true,0);
	sendGPRScmd("AT+CSTT=\"epc.tmobile.com\"","OK",10000,true,0);
//	sendGPRScmd("AT+CSTT=\"telnamobile.com\"","OK",10000,true,0);

	startGPRS(10000);
	
//	sendGPRScmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",20000,true,0); 
//	sendGPRScmd("AT+SAPBR=3,1,\"APN\",\"epc.tmobile.com\"","OK",20000,true,0);
//	sendGPRScmd("AT+SAPBR=3,1,\"APN\",\"telnamobile.com\"","OK",20000,true,0);

//	sendGPRScmd("AT+SAPBR=4,1","OK",20000,true,0); //checks configuration
	
//	sendGPRScmd("AT+SAPBR=1,1","OK",20000,true,0);// Tries to connect GPRS 
	sendGPRScmd("AT+CGATT?","OK",20000,true,0);// GPRS Status
//	sendGPRScmd("AT+SAPBR=2,1","OK",20000,true,0);// Checks Status of GPRS 
	delay(5000);
	sendCoordinates();
	
//	sendGPRScmd("AT+SAPBR=0,1","OK",20000,true,0);// Disconnect GPRS
	*/
	
	
	sim900.gsmSleepMode2();
}


void startGPRS(unsigned long Timeout){
    sendGPRScmd ("AT+CIICR","OK",Timeout,true,0); 
    sendGPRScmd ("AT+CSNS=4","OK",Timeout,true,0);
    sendGPRScmd ("AT+CIFSR",".",Timeout,true,0);   
}

void buildGPSstring(){

    char eepChar;
    for (uint8_t ep = 0; ep < 25; ep++)
    {
      eepChar = EEPROM.read(ep + GEOGRAMONEID);
      if(eepChar == '\0')
        break;
      else
        GSM.print(eepChar);
    }

    GSM.print("~");
    sim900.printLatLon(&lastValid.latitude,&lastValid.longitude);
    GSM.print("~");
    GSM.print(MAX17043getBatterySOC()/100,DEC);
    GSM.print("~");
    GSM.print(lastValid.month,DEC);
    GSM.print("/");
    GSM.print(lastValid.day,DEC);
    GSM.print("/");
    GSM.print(lastValid.year,DEC);
    GSM.print(" ");
    GSM.print(lastValid.hour,DEC);
    GSM.print(":");
    GSM.print(lastValid.minute,DEC);
    GSM.print(":");
    GSM.print(lastValid.seconds,DEC);
    GSM.print(lastValid.amPM);
    GSM.print("~");
    GSM.print(lastValid.speed);
    
    
}
void sendCoordinates () {
	
    //check GPRS IP address and try to reconnect if none
/*    boolean hasIPaddress;
    hasIPaddress=sendGPRScmd ("AT+CIFSR",".",10000,true,0);
    if(hasIPaddress==false){
        startGPRS(10000);  
    }  
 */ 
  
    sendGPRScmd ("AT+CIPSTART=\"UDP\",\"172.30.0.212\",\"12345\"","T OK",20000,true,0);
    sendGPRScmd ("AT+CIPSEND=4",">",30000,true,0);
	GSM.print("test");
//	sendGPRScmd ("AT+CIPSEND",">",30000,true,0);
//    buildGPSstring();
//	GSM.println("$FRLIN,IMEI,012896006334665,*48");
	GSM.print(0x1A,BYTE);//send the data to the server
    delay(500);
//	sendGPRScmd ("AT+CIPSEND",">",30000,true,0);
//	GSM.println("$GPRMC,154403.000,A,6311.64120,N,01438.02740,E,0.000,0.0,270707,,*0A");

//    GSM.print(0x1A,BYTE);//send the data to the server
//    delay(500);
    sendGPRScmd ("AT+CIPCLOSE","OK",20000,true,0);

}

void ShowSerialData()
{
  //while(GSM.available()!=0)
    //Serial.write(GSM.read());
}

boolean sendGPRScmd (char *cmd, char *waitFor, unsigned long atTimeOut, boolean pnl, int tries) {
    //delay(1000); 
	//sim900.gsmSleepMode0();
    char atCommand[55];// was 16 
    strcpy(atCommand,cmd);
    unsigned long atimeOut=millis();
    char buffer[200];
    int i=0;  
  
    //Serial.println("START");
    GSM.flush();
    if(pnl)GSM.println(cmd);
    else GSM.print(cmd);
    
/*    if(tries>=3){
      Serial.println("3 TRIES - FAILED"); 
      return false;   
    }
 */   
    tries++; 
    while ((millis() - atimeOut) <= atTimeOut){
      if(GSM.available()){
       // Serial.write(GSM.read());
        buffer[i]=GSM.read();
//       Serial.write(buffer[i]);
        i++;
        buffer[i] = '\0';
        if(strstr(buffer,atCommand)!=NULL){
          i=0;
          buffer[i]='\0';
        }
        if(strstr(buffer,"ERROR")!=NULL){ //if there is an error send AT command again
//          Serial.println("RETRY");
          return 0;//sendGPRScmd (cmd,waitFor,atTimeOut,pnl,tries);
        }
        if(strstr(buffer,waitFor)!=NULL){ 
//           Serial.println("FOUND");
            return true;
        }
      }
    }
//    Serial.println("TIMEOUT");
//    Serial.println("RETRY");
//    return sendGPRScmd (cmd,waitFor,atTimeOut,pnl,tries);
}
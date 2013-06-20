/*****************************************************************************
The Geogram ONE is an open source tracking device/development board based off 
the Arduino platform.  The hardware design and software files are released 
under CC-SA v3 license.
*****************************************************************************/

#include <AltSoftSerial.h>
#include <PinChangeInt.h>
#include "GeogramONE.h"
#include <EEPROM.h>
#include <I2C.h>
#include "eepromAnything.h"

#define USEFENCE1	0  //set to zero to free up code space if option is not needed
#define USEFENCE2	0  //set to zero to free up code space if option is not needed
#define USEFENCE3	0  //set to zero to free up code space if option is not needed
#define USESPEED	0  //set to zero to free up code space if option is not needed
#define USEMOTION	0  //set to zero to free up code space if option is not needed

GeogramONE ggo;
AltSoftSerial GSM;
SIM900 sim900(&GSM);
geoSmsData smsData;
PA6C gps(&Serial); 
goCoord lastValid;
geoFence fence;


volatile uint8_t call;
volatile uint8_t move;
volatile uint8_t battery = 0;
volatile uint8_t charge = 0x02; // force a read of the charger cable
volatile uint8_t d4Switch = 0x00;
volatile uint8_t d10Switch = 0x00;

uint8_t cmd0 = 0;
uint8_t cmd1 = 0;
uint8_t cmd3 = 0;
uint8_t udp = 0x00; 

//#if USEFENCE1
uint8_t fence1 = 0;
//#endif
//#if USEFENCE2
uint8_t fence2 = 0;
//#endif
//#if USEFENCE3
uint8_t fence3 = 0;
//#endif

uint8_t breachSpeed = 0;
uint8_t breachReps = 0;

uint32_t smsInterval = 0;
uint32_t udpInterval = 0;
uint32_t sleepTimeOn = 0;
uint32_t sleepTimeOff = 0;
uint8_t sleepTimeConfig = 0;

uint8_t speedHyst = 0;
uint16_t speedLimit = 0;

char udpReply[11];


unsigned long miniTimer = millis();

void goesWhere(char *, uint8_t replyOrStored = 0);
  
void setup()
{
	ggo.init();
	gps.init(115200);
	sim900.init(9600);
        
	MAX17043init(7, 500);
	BMA250init(3, 500);

	attachInterrupt(0, ringIndicator, FALLING);
	attachInterrupt(1, movement, FALLING);
	PCintPort::attachInterrupt(PG_INT, &charger, CHANGE);
	PCintPort::attachInterrupt(FUELGAUGEPIN, &lowBattery, FALLING);

	goesWhere(smsData.smsNumber);
	call = sim900.checkForMessages();
	if(call == 0xFF)
		call = 0;
	battery = MAX17043getAlertFlag();
	#if USEFENCE1
	ggo.getFenceActive(1, &fence1);
	#endif
	#if USEFENCE2
	ggo.getFenceActive(2, &fence2);
	#endif
	#if USEFENCE3
	ggo.getFenceActive(3, &fence3);
	#endif
	#if USESPEED
	ggo.configureSpeed(&cmd3, &speedHyst, &speedLimit);
	#endif
	ggo.configureBreachParameters(&breachSpeed, &breachReps);
	ggo.configureInterval(&smsInterval, &sleepTimeOn, &sleepTimeOff, &sleepTimeConfig, &udpInterval);
	if(sleepTimeConfig & 0x02)
		BMA250enableInterrupts();
	uint8_t swInt = EEPROM.read(IOSTATE0);
	if(swInt == 0x05)
		PCintPort::attachInterrupt(4, &d4Interrupt, RISING);
	if(swInt == 0x06)
		PCintPort::attachInterrupt(4, &d4Interrupt, FALLING);
	swInt = EEPROM.read(IOSTATE1);
	if(swInt == 0x05)
		PCintPort::attachInterrupt(10, &d10Interrupt, RISING);
	if(swInt == 0x06)
		PCintPort::attachInterrupt(10, &d10Interrupt, FALLING);
     
    miniTimer = 0;

}


void httpPost()
{
    // Are we connected to the GPS?     
	if(!lastValid.signalLock) {
		return ;
    }
    
    if( millis() - miniTimer < 1000*10 ) {
        // We are trying to poll sooner then 10 secs 
        return  ; // Nothing to do. 
    }
    miniTimer = millis() ; 
    
	sim900.gsmSleepMode(0);
	
	GSM.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+SAPBR=3,1,\"APN\",\"internet.com\""); //need to put your provider's APN here
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+SAPBR=1,1");
	sim900.confirmAtCommand("OK",5000);// Tries to connect GPRS 
	
	GSM.println("AT+HTTPINIT");
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+HTTPPARA=\"CID\",1");
	sim900.confirmAtCommand("OK",5000);
	
	GSM.print("AT+HTTPPARA=\"URL\",\"http://www.abluestar.com/temp/gps/?id=123"); //web address to send data to
     if( lastValid.signalLock ) {
          GSM.print("&lat=");
          if(lastValid.ns == 'S') {
              GSM.print("-");
          }
          GSM.print(lastValid.latitude[0]);
          GSM.print(lastValid.latitude[1]);
          GSM.print("+");
          GSM.print(lastValid.latitude + 2);
          
          GSM.print("&lon=");
          if(lastValid.ew == 'W') {
              GSM.print("-");
          }
      	  GSM.print(lastValid.longitude[0]);
          GSM.print(lastValid.longitude[1]);
          GSM.print(lastValid.longitude[2]);
          GSM.print("+");
          GSM.print(lastValid.longitude + 3);

          
          GSM.print("&alt=");
          GSM.print(lastValid.altitude);  
        
          GSM.print("&sat=");
          GSM.print(lastValid.satellitesUsed);          
          
          GSM.print("&speed=");
          GSM.print(lastValid.speed);  
          
          GSM.print("&batp=");          
          GSM.print(MAX17043getBatterySOC()/100);  
          
          GSM.print("&batv=");          
          GSM.print( MAX17043getBatteryVoltage()/1000.0, 2 );  
          
          GSM.print("&dir=");
          GSM.print(lastValid.courseDirection);  
          
          
          // YYYY-MM-DD
          GSM.print("&date=");          
          GSM.print(lastValid.date + 4 );  
          GSM.print("-" );  
          GSM.print(lastValid.date[2] );  
          GSM.print(lastValid.date[3] );  
          GSM.print("-" );  
          GSM.print(lastValid.date[0] );  
          GSM.print(lastValid.date[1] );  
          
          // HH::MM::SS
          GSM.print("&time=");
          GSM.print(lastValid.time[0] );  
          GSM.print(lastValid.time[1] );  
          GSM.print("-" );  
          GSM.print(lastValid.time[2] );  
          GSM.print(lastValid.time[3] );  
          GSM.print("-" );  
          GSM.print(lastValid.time +4 );  
          
    } else {
        GSM.print("&err=NoSignalLock");
    }    
    GSM.println("\"");    
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+HTTPDATA=10,10000"); //100 refers to how many bytes you're sending.  You'll probably have to tweak or just put a large #
	sim900.confirmAtCommand("DOWNLOAD",5000);
	
	GSM.println("0123456789"); //ie latitude,longitude,etc...
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+HTTPACTION=1"); //POST the data
	sim900.confirmAtCommand("ACTION:",5000);
	
	GSM.println("AT+HTTPTERM"); //terminate http
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+SAPBR=0,1");// Disconnect GPRS
	sim900.confirmAtCommand("OK",5000);
	sim900.confirmAtCommand("DEACT",5000);
	
	sim900.gsmSleepMode(2);
}


#if 0 
boolean DoHTTP()
{    
    // Are we connected to the GPS?     
	if(!lastValid.signalLock) {
		return false;
    }
    
    
    if( millis() - miniTimer < 1000*10 ) {
        // We are trying to poll sooner then 10 secs 
        return false ; // Nothing to do. 
    }
           
    static bool sendOK = false;  
	if(!sendOK)	
    {        
        // sim900.confirmAtCommand("OK",500);
        if(!sim900.signalQuality()) {
            // No signal to the Cell towers. 
            return false;
        }
        if(!sim900.checkNetworkRegistration())
        {
            // Check the avliable networks
            GSM.println("AT+CGATT?");	
            if(sim900.confirmAtCommand("OK",10000) != 0) {
                // GSM.println("Error: Could not get the available networks. ");
                return false; 
            }
        
            // Perform a GPRS Attach. The device should be attached to the GPRS network before a PDP context can be established
            GSM.println("AT+CGATT=1");	
            if(sim900.confirmAtCommand("OK",10000) != 0) 
            {
                // ERROR, need to reboot GSM module
                static unsigned long resetGSM = millis();
                
                // if more than 5 minutes reboot GSM module
                if((millis() - resetGSM) >= 300000) 
                {
                    sim900.powerDownGSM();
                    sim900.init(9600);
                    resetGSM = millis();
                    return false; // Try again. 
                }
            }
        } else {
            return false; // Not sent but we need to re-run this command. 
        }
		
        /*
        GSM.println("AT+SAPBR=0,1");
        if ( sim900.confirmAtCommand("OK",3000) != 0) {
            GSM.println("Error: Could not set the profile to 0");
            return false;
        }        
        */
        
        
        // defines connection type (command 3 (set), profile 1)
        GSM.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        if ( sim900.confirmAtCommand("OK",10000) != 0) {        
            // GSM.println("Error: Could not define the connectin type"); 
            return false;
        }
        
        // Connect to the APN 
        GSM.println("AT+SAPBR=3,1,\"APN\",\"internet.com\"");
        if( sim900.confirmAtCommand("OK",3000) != 0 ) {
            // GSM.println("Error: Could not set the APN");
            return false ;
        }
        
        // Opens GPRS connection using profile 1, may return OK or ERROR
        // I put the "AT+SAPBR=1,1" command to be always called each time because I noticed, on long run testings, that GPRS connection may drop accidentally, so basically I bruteforce the connection.
        GSM.println("AT+SAPBR=1,1");
        if ( sim900.confirmAtCommand("OK",3000) != 0) {
            // GSM.println("Error: Could not set the profile to 1");
            return false;
        }
	}
    
    // Ready to send the message. 
    GSM.flush();
    
   // initializes embedded HTTP rutine, return OK or ERROR
    GSM.println("AT+HTTPINIT");
    if ( sim900.confirmAtCommand("OK",3500) != 0 ) {
        // GSM.println("Error: initializes embedded HTTP rutine");
        sendOK = false; 
        return sendOK;
    }         
    
    GSM.println("AT+HTTPPARA=\"CID\", \"1\"");
    if ( sim900.confirmAtCommand("OK",10000) != 0 ) {
        //GSM.println("Error: Set the CID");
        sendOK = false; 
        return sendOK;
    }     
        
    // Issue an HTTP Url Request
    GSM.print("AT+HTTPPARA=\"URL\",\"http://www.abluestar.com/temp/gps/?");
    
    
    
    GSM.print("id=123&");    
    if( lastValid.signalLock ) {
          
          long latitude = (long) lastValid.latitude ; 
          long longitude = (long) lastValid.longitude ; 
        
          GSM.print("lat=");
          GSM.print(abs( latitude/1000000) );
          GSM.print("+");
          GSM.print(abs(( latitude % 1000000) / 10000.0), 4);    
          // GSM.print(lastValid.latitude);
          
          GSM.print("&lon=");
          GSM.print(abs( longitude / 1000000));
          GSM.print("+");
          GSM.print(abs(( longitude % 1000000) / 10000.0), 4);    
          //GSM.print(lastValid.longitude);
          
          /*GSM.print("&satellitesUsed=");
          GSM.print(lastValid.satellitesUsed);
          GSM.print("&altitude=");
          GSM.print(lastValid.altitude);  
          GSM.print("&speed=");
          GSM.print(lastValid.speed);  
          // GSM.print("&battery=");
          // GSM.print(lastValid.battery);  
          // GSM.print("&direction=");
          // GSM.print(lastValid.direction);  
          GSM.print("&date=");
          GSM.print(lastValid.date);  
          GSM.print("&time=");
          GSM.print(lastValid.time);  
          */
          
    } else {
        GSM.print("err=NoSignalLock");
    }    
    GSM.println("\"");
    if ( sim900.confirmAtCommand("OK",10000) != 0 ) {
        //GSM.println("Error: Set the url to be polled");
        sendOK = false; 
        return sendOK;
    }

    // gets the url by GET method
    GSM.println("AT+HTTPACTION=0");
    if ( sim900.confirmAtCommand("OK",15000) != 0 ) {
        //GSM.println("Error: Could not request the url");
        sendOK = false; 
        return sendOK;
    }
    delay( 1000); 

    GSM.println("AT+HTTPREAD");
    
    delay( 3000 );
    GSM.flush();
   
     
    GSM.println("AT+HTTPTERM");
    if ( sim900.confirmAtCommand("OK",15000) != 0 ) {
        //GSM.println("Error: Could not end the HTTP session. ");
        sendOK = false; 
        return sendOK;
    }  
   
    GSM.println("");
    GSM.flush();
    
    // Reset the timer 
    miniTimer = millis() ; 
    
    // Everything looks good to me. 
    sendOK = true;
    return sendOK; 
}
#endif 


void loop()
{

  // Update the GPS coordinates if possiable
  if(!gps.getCoordinates(&lastValid))
  {
    int8_t tZ = EEPROM.read(TIMEZONE);
    bool eM   = EEPROM.read(ENGMETRIC);
    gps.updateRegionalSettings(tZ, eM, &lastValid);
  }

  // Check and update the webserver with the GPS cords.     
  httpPost() ; 
  // DoHTTP();  
       

	if(call)
	{
		sim900.gsmSleepMode(0);
		char pwd[5];
		EEPROM_readAnything(PINCODE,pwd);
		if(sim900.signalQuality())
		{
			if(!sim900.getGeo(&smsData, pwd))
			{
				if(!smsData.smsPending)
					call = 0; // no more messages
				if(smsData.smsDataValid)
				{
					if(!smsData.smsCmdNum)
						cmd0 = 0x01;
					else if(smsData.smsCmdNum == 1)
						cmd1 = 0x01;
					else if(smsData.smsCmdNum == 2)
						command2();
					else if(smsData.smsCmdNum == 3)
						cmd3 = 0x01;
					else if(smsData.smsCmdNum == 4)
						command4();
					else if(smsData.smsCmdNum == 5)
						command5();
					else if(smsData.smsCmdNum == 6)
						command6();
					else if(smsData.smsCmdNum == 7)
						command7();
					else if(smsData.smsCmdNum == 8)
						command8();
				}
			}
		}
		sim900.gsmSleepMode(2);	
	}
	if(cmd0)
		command0();
	#if USEMOTION
	if(cmd1)
		command1();
	#endif
	#if USESPEED
	if(cmd3)
		command3();
	#endif
	if(udp)
		udpOrange();
	if(battery)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(BATTERYMSG);
			if(!sim900.sendSMS())
			{
				battery = 0;
				MAX17043clearAlertFlag();
			}
		}
		sim900.gsmSleepMode(2);
	}
	if(charge & 0x02)
		chargerStatus();
	#if USEFENCE1
	if(fence1)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach1Conf = 0;
		static char previousSeconds1 = lastValid.time[5];
		if((fence1 == 1) && (lastValid.speed >= breachSpeed))
		{
			ggo.configureFence(1,&fence); 
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds1)
					breach1Conf++;
				if(breach1Conf > breachReps)
				{
					fence1 = 2;
					breach1Conf = 0;
				}
				previousSeconds1 = lastValid.time[5];
			}
			else
				breach1Conf = 0;
		}
		else
			breach1Conf = 0;
		if(fence1 == 2)
		{
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE1MSG);
				if(!sim900.sendSMS())
					fence1 = 0x00;
			}
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	#if USEFENCE2
	if(fence2)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach2Conf = 0;
		static char previousSeconds2 = lastValid.time[5];
		if((fence2 == 1) && (lastValid.speed >= breachSpeed))
		{  
			ggo.configureFence(2,&fence);
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds2)
					breach2Conf++;
				if(breach2Conf > breachReps)
				{
					fence2 = 2;
					breach2Conf = 0;
				}
				previousSeconds2 = lastValid.time[5];
			}
			else
				breach2Conf = 0;
		}
		else
			breach2Conf = 0;
		if(fence2 == 2)
		{  
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE2MSG);
				if(!sim900.sendSMS())
					fence2 = 0x00;
			}
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	#if USEFENCE3
	if(fence3)
	{
		bool engMetric = EEPROM.read(ENGMETRIC);
		static uint8_t breach3Conf = 0;
		static char previousSeconds3 = lastValid.time[5];
		if((fence3 == 1) && (lastValid.speed >= breachSpeed))
		{  
			ggo.configureFence(3,&fence);
			if(!gps.geoFenceDistance(&lastValid, &fence, engMetric))
			{
				if(lastValid.time[5] != previousSeconds3)
					breach3Conf++;
				if(breach3Conf > breachReps)
				{
					fence3 = 2;
					breach3Conf = 0;
				}
				previousSeconds3 = lastValid.time[5];
			}
			else
				breach3Conf = 0;
		}
		else
			breach3Conf = 0;
		if(fence3 == 2)
		{  
			sim900.gsmSleepMode(0);
			goesWhere(smsData.smsNumber);
			if(!sim900.prepareSMS(smsData.smsNumber))
			{
				printEEPROM(FENCE3MSG);
				if(!sim900.sendSMS())
					fence3 = 0x00;
			}	
			sim900.gsmSleepMode(2);
		}
	}
	#endif
	if(smsInterval)
		smsTimerMenu();
	if(udpInterval)
		udpTimerMenu();
	if(sleepTimeOn && sleepTimeOff)
		sleepTimer();
	if(d4Switch)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(D4MSG);
			if(!sim900.sendSMS())
				d4Switch = 0x00;
		}
		sim900.gsmSleepMode(2);
	}
	if(d10Switch)
	{
		sim900.gsmSleepMode(0);
		goesWhere(smsData.smsNumber);
		if(!sim900.prepareSMS(smsData.smsNumber))
		{
			printEEPROM(D10MSG);
			if(!sim900.sendSMS())
				d10Switch = 0x00;
		}
		sim900.gsmSleepMode(2);
	}


        // sim900.PingHTTP(666, 555, 333);
} 

void printEEPROM(uint16_t eAddress)
{
	char eepChar;
	for (uint8_t ep = 0; ep < 50; ep++)
	{
		eepChar = EEPROM.read(ep + eAddress);
		if(eepChar == '\0')
			break;
		else
			GSM.print(eepChar);
	}
}

void goesWhere(char *smsAddress, uint8_t replyOrStored)
{
	if(!replyOrStored)
		EEPROM_readAnything(RETURNADDCONFIG,replyOrStored);
	if((replyOrStored == 2) || ((replyOrStored == 3) && (smsAddress[0] == NULL)))
	for(uint8_t l = 0; l < 39; l++)
	{
			smsAddress[l] = EEPROM.read(l + SMSADDRESS);
			if(smsAddress[l] == NULL)
				break;
	}
}




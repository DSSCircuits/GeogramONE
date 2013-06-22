/*****************************************************************************
The Geogram ONE is an open source tracking device/development board based off 
the Arduino platform.  The hardware design and software files are released 
under CC-SA v3 license.
*****************************************************************************/
/*****************************************************************************
WARNING: 
This file has been updated by Steven Smethurst (funvill) to ping a webserver 
with GPS location, and Sim card identity periodically. If you run this code 
unaltered, you will send your personal information to Stevens webserver. 

To learn more about these changes please visit my website. 
http://www.abluestar.com

Notes: 
 * GSM modem manual (Sim900): http://garden.seeedstudio.com/images/a/a8/SIM900_AT_Command_Manual_V1.03.pdf
*****************************************************************************/

#include <AltSoftSerial.h>
#include <PinChangeInt.h>
#include "GeogramONE.h"
#include <EEPROM.h>
#include <I2C.h>

// You should change this to your own server. but you can use this setting 
// for a demo. 
#define SETTING_WEBSERVER_URL       "http://www.abluestar.com/temp/gps/"

// Need to put your provider's APN here
#define SETTING_GSM_APN             "internet.com"

/**
 * HTTP is an expencive protocol that consumed a lot of bytes in the header. 
 * A HTTP packet with data will take a min of 240 BYTES up to 300 BYTES 
 * 240 == 53( TCP overhread) 120 BYTES (URL) 17 (Domain) 50 (HTTP overhead) 
 * 
 * The more frequent that you poll the server the more data you are going to 
 * use. For example if we where to send a packet with a length of 300 BYTES. 
 *      Poll Frequency  | Times in a day  | Data per month (30 days) 
 *      --------------------------------------------------
 *           10 sec     |     8640        |    75 MB
 *           60 sec     |     1440        |    12 MB
 *           15 min     |       96        |   840 KB 
 *           30 min     |       48        |    42 KB
 *            1 hr      |       24        |    21 KB 
 *
 * Only sending packets when something changes greatly reduces the amount of 
 * bandwidth needed. (normaly we are only moving 1/3 of the day max) 
 *
 * 
 */ 


#define TIMER_POLL_WITH_DATA        1000*60*1
#define TIMER_POLL_HEART_BEAT       1000*60*60
#define ENABLE_DEBUG_MESSAGES       false 

GeogramONE      ggo;
AltSoftSerial   GSM;
SIM900          sim900(&GSM);
PA6C            gps(&Serial); 
goCoord         lastValid;


#define MAX_PHONENUMBER_SIZE 25 
char phoneNumber[MAX_PHONENUMBER_SIZE];

unsigned long   miniTimer ; 

void DebugPrint( char * msg) {
    if( ! ENABLE_DEBUG_MESSAGES ) {
        return ; 
    }    
    Serial.println( msg ); 
}



void httpPost()
{
    // ToDo: only poll the GPS when we need to. 
    // ToDo: shut down the GPS if we don't need it. 
    // Update the GPS coordinates if possiable
    gps.getCoordinates(&lastValid); 

    if( millis() - miniTimer < TIMER_POLL_WITH_DATA ) {
        // We are trying to poll sooner then we should.
        return  ; // Nothing to do. 
    }
    
    // Are we connected to the GPS?   
    if( ! lastValid.signalLock  ) {
        // We are not connected to the GPS. 
        // Has our heart beat timer expired? 
        if( millis() - miniTimer < TIMER_POLL_HEART_BEAT ) {
            // We are trying to poll sooner then we should.
            return  ; // Nothing to do. 
        }  
    } 
    miniTimer = millis() ; 
    
    
    
    // Wake up the modem. 
    // DebugPrint( "Waiting up the GSM modem"); 
	sim900.gsmSleepMode(0);
    
	GSM.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
	sim900.confirmAtCommand("OK",5000);
	
	GSM.print("AT+SAPBR=3,1,\"APN\",\"");
    GSM.print( SETTING_GSM_APN );
    GSM.println("\""); 
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+SAPBR=1,1");
	sim900.confirmAtCommand("OK",5000);// Tries to connect GPRS 
	
	GSM.println("AT+HTTPINIT");
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+HTTPPARA=\"CID\",1");
	sim900.confirmAtCommand("OK",5000);
	
    //web address to send data to
	GSM.print("AT+HTTPPARA=\"URL\",\"");
    GSM.print(SETTING_WEBSERVER_URL);
    GSM.print("?id="); 
    GSM.print(phoneNumber); 
    
    // If we have GPS lock we should send the GPS data. 
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
          
          GSM.print("&dir=");
          GSM.print(lastValid.courseDirection);  
          
          /*
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
          */
          // Raw
          /*
          GSM.print("&raw1=");
          for( int i = 0 ; i < 10 ; i++ ) {
            GSM.print(lastValid.latitude[i], HEX);
          }
          GSM.print("&raw2=");
          for( int i = 0 ; i < 11 ; i++ ) {
            GSM.print(lastValid.longitude[i], HEX);
          }
          */
          
    } else {
        GSM.print("&err=NoSignalLock");
    }    
    
    // Get the battery state 
      GSM.print("&batp=");          
      GSM.print(MAX17043getBatterySOC()/100);  
      
      GSM.print("&batv=");          
      GSM.print( MAX17043getBatteryVoltage()/1000.0, 2 );      
    
    // All done send the message. 
    GSM.println("\"");    
	sim900.confirmAtCommand("OK",5000);
	
    
	GSM.println("AT+HTTPDATA=2,10000"); 
	sim900.confirmAtCommand("DOWNLOAD",5000);
	GSM.println("0"); // ToDo: But prameters here instead of the url. 
	
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+HTTPACTION=1"); //POST the data
	sim900.confirmAtCommand("ACTION:",5000);
	
    delay (3000); 
	GSM.println("AT+HTTPTERM"); //terminate http
	sim900.confirmAtCommand("OK",5000);
	
	GSM.println("AT+SAPBR=0,1");// Disconnect GPRS
	sim900.confirmAtCommand("OK",5000);
	sim900.confirmAtCommand("DEACT",5000);
	
    // Put the modem to sleep.
	sim900.gsmSleepMode(2);
}

/**
 * Gets the phone number from the device if possible
 * 
 * Request:
 *          AT+CNUM
 *
 * Response: 
 *          +CNUM: "","+11234567890",145,7,4
 *          OK
 */ 
uint8_t GetPhoneNumber() {

    // Request the phone number from the sim card 
    GSM.println("AT+CNUM");
    
    // Wait for a response. 
	if( sim900.confirmAtCommand("OK",5000) == 0 ) {
        // Extract Phone number from the response. 
        // Search for the start of the string. 
        char * startOfPhoneNumber = strstr( sim900.atRxBuffer, "\",\"" ); 
        if( startOfPhoneNumber != NULL ) { 
            // Found the start of the string 
            startOfPhoneNumber += 3 ; // Move past the header. 
            if( startOfPhoneNumber[0] == '+' ) {
                startOfPhoneNumber++; // Move past the plus
            }
            char * endOfPhoneNumber = strstr( startOfPhoneNumber, "\"" ); 
            if( endOfPhoneNumber != NULL ) {
                // Found the end of the string. 
                if( endOfPhoneNumber - startOfPhoneNumber < MAX_PHONENUMBER_SIZE-1 ) {                
                    // Fits in the buffer 
                    strncpy( phoneNumber, startOfPhoneNumber, endOfPhoneNumber - startOfPhoneNumber ) ;                
                    phoneNumber[ endOfPhoneNumber - startOfPhoneNumber ] = 0 ; 
                    return 1; 
                }
            }
        }
    }
    return 0; 
}

void setup() {
	ggo.init();
	gps.init(115200);
	sim900.init(9600);
        
	MAX17043init(7, 500);
	BMA250init(3, 500);
    
    // Get the phone number from the simcard
    GetPhoneNumber(); 
    
    // Reset the timer 
    miniTimer = 0 ; 
}

void loop() {
    // Ping the webserver. 
    httpPost() ; 
} 
#include "GeogramONE.h"

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}
 

GeogramONE::GeogramONE()
{
}

void GeogramONE::goToSleep(uint8_t whichMode)
{
	delay(200);  //need delay because GSM module is shutting down. 1000 works
	uint8_t pcicrReg = PCICR; //backup the current Pin Change Interrupt register
	PCICR = 0; // disable all pin change interrupts. Need to do this because of NewSoftSerial

	pinMode(9,INPUT);  //shut off NewSoftSerial Tx pin 
	digitalWrite(9,LOW); //set to high impedance
	digitalWrite(8,LOW); // set NewSoftSerial Rx pin to high impedance

	ADCSRA = 0; 
	if(!whichMode)
	{
		set_sleep_mode (SLEEP_MODE_PWR_DOWN);
		sleep_enable();	
		MCUCR = _BV (BODS) | _BV (BODSE);
		MCUCR = _BV (BODS);
		sleep_cpu ();
	}
	else
	{
		set_sleep_mode (SLEEP_MODE_PWR_SAVE);
		sleep_enable();	
		resetTimer2();
		MCUCR = _BV (BODS) | _BV (BODSE);
		MCUCR = _BV (BODS);
		sleep_cpu ();
		TCCR2B = 0x00; //set to zero so timer does not start counting yet
	}
  
/*********ATMEGA is sleeping at this point***************/  
	sleep_disable();

	pinMode(9,OUTPUT); //restore NewSoftSerial settings
	digitalWrite(9,HIGH);
	digitalWrite(8,HIGH);
	PCICR = pcicrReg; //restore Pin Change Interrupt register
}

uint8_t GeogramONE::init()
{
/*	char batteryMsg[25] = "Low Battery Alert";
	char motionMsg[25] = "Motion Detected";
	char fence1Msg[25] = "Fence 1 Breach";
	char fence2Msg[25] = "Fence 2 Breach";
	char fence3Msg[25] = "Fence 3 Breach";
	EEPROM_writeAnything(BATTERYMSG,batteryMsg);
	EEPROM_writeAnything(MOTIONMSG,motionMsg);
	EEPROM_writeAnything(FENCE1MSG,fence1Msg);
	EEPROM_writeAnything(FENCE2MSG,fence2Msg);
	EEPROM_writeAnything(FENCE3MSG,fence3Msg);
*/

	configureIO(2,IOSTATE0); //D2
	configureIO(10,IOSTATE0);//D10
	configureIO(15,IOSTATE0);//A1
	configureIO(16,IOSTATE0);//A2
	configureIO(17,IOSTATE0);//A3
	configureIO(20,IOSTATE0);//A6
}

uint8_t GeogramONE::configureIO(uint8_t pinNumber, uint8_t eepromAddress)
{
	uint8_t ioConfig = 0;
	ioConfig = EEPROM.read(eepromAddress);
	if(ioConfig == 0){pinMode(pinNumber,INPUT);digitalWrite(pinNumber,LOW);}
	if((ioConfig == 1) || (ioConfig == 5) || (ioConfig == 6) || (ioConfig == 7)){pinMode(pinNumber,INPUT);digitalWrite(pinNumber,HIGH);}
	if(ioConfig == 2){pinMode(pinNumber,OUTPUT);digitalWrite(pinNumber,LOW);}
	if(ioConfig == 3){pinMode(pinNumber,OUTPUT);digitalWrite(pinNumber,HIGH);}
	if(ioConfig == 4){analogReference(DEFAULT);analogRead(pinNumber);}
}

uint8_t GeogramONE::resetTimer2( )
{
	TIFR2 = 0x00; //clear timer2 overflow flag
	TCNT2 = 0x00;
	TCCR2A = 0x00;
	TCCR2B = 0x00; //set to zero so timer does not start counting yet
	TCNT2 = 0x00; //reset counter back to zero
	ASSR &= ~(1<<AS2);
	TIMSK2 |= (1<<TOIE2);
	TIMSK2 &= ~((1<<OCIE2A)|(1<<OCIE2B));
	TCCR2B = 0x07; //start counting, 0x07 
	//TIFR2 = 0x00; //clear timer2 overflow flag
}

uint8_t GeogramONE::startTimer2( unsigned long secondTime )
{
	//secondTime /= 0.032768;
	
	for ( uint32_t tc = 0; tc < secondTime; tc++ )
	{
		resetTimer2( );
		goToSleep(1);
		TCCR2B = 0x00; //set to zero so timer does not start counting yet
		TIFR2 = 0x00; //clear timer2 overflow flag
	}
}


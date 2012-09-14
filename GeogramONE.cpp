#include "GeogramONE.h"

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
	configureIO(2,IOSTATE0); //D2
	configureIO(10,IOSTATE1);//D10
	configureIO(15,IOSTATE2);//A1
	configureIO(16,IOSTATE3);//A2
	configureIO(17,IOSTATE4);//A3
	configureIO(20,IOSTATE5);//A6
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

void GeogramONE::configureMAX17043(uint8_t *battery)
{  
	EEPROM_readAnything(BATTERYLOWLEVEL,*battery);
}

void GeogramONE::configureBMA250(registersBMA250 *config)
{
	EEPROM_readAnything(BMA0X0F,config->zeroF);
	EEPROM_readAnything(BMA0X10,config->oneZero);
	EEPROM_readAnything(BMA0X11,config->oneOne);
	EEPROM_readAnything(BMA0X16,config->oneSix);
	EEPROM_readAnything(BMA0X17,config->oneSeven);
	EEPROM_readAnything(BMA0X19,config->oneNine);
	EEPROM_readAnything(BMA0X1A,config->oneA);	
	EEPROM_readAnything(BMA0X1B,config->oneB);
	EEPROM_readAnything(BMA0X20,config->twoZero);
	EEPROM_readAnything(BMA0X21,config->twoOne);
	EEPROM_readAnything(BMA0X25,config->twoFive);
	EEPROM_readAnything(BMA0X26,config->twoSix);
	EEPROM_readAnything(BMA0X27,config->twoSeven);
	EEPROM_readAnything(BMA0X28,config->twoEight);
}

void GeogramONE::configurePA6C(configVar *settings)
{
	EEPROM_readAnything(ENGMETRIC,settings->engMetric);
	EEPROM_readAnything(AMPM,settings->amPm);
	EEPROM_readAnything(TIMEZONE,settings->timeZone);
}

void GeogramONE::configureFence(uint8_t fenceNumber, geoFence *fence)
{
	uint8_t offset = 0;
	if(fenceNumber == 2)
		offset += 15;
	if(fenceNumber == 3)
		offset += 30;
	EEPROM_readAnything((INOUT1 + offset), fence->inOut);
	EEPROM_readAnything((RADIUS1 + offset), fence->radius);
	EEPROM_readAnything((LATITUDE1 + offset), fence->latitude);
	EEPROM_readAnything((LONGITUDE1 + offset), fence->longitude);
	return ;
}

void GeogramONE::getFenceActive(uint8_t fenceNumber, uint8_t *fenceVar)
{
	uint8_t offset = 0;
	if(fenceNumber == 2)
		offset += 15;
	if(fenceNumber == 3)
		offset += 30;
	EEPROM_readAnything(ACTIVE1 + offset,*fenceVar);
	return ;
}

void GeogramONE::configureInterval(uint32_t *timeInterval)
{
	EEPROM_readAnything(SENDINTERVAL,*timeInterval);
	return;
}
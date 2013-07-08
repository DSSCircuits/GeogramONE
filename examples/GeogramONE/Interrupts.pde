void ringIndicator()
{
	call = 1;
}

void movement()
{
	move = 0xFF;
}

void charger()
{
	charge |= 0x02;
}

void lowBattery()
{
	battery = 1;
}

/*************************************************************	
	Procedure to check the status of the USB charging cable. 
	If the charging cable is plugged in  charge variable 
	will be a 0x01.  Unplugged, charge variable is 0x00.

**************************************************************/
void chargerStatus()
{
	delay(2);
	charge = digitalRead(PG_INT);
	if(charge)
	{
		smsInterval = EEPROM.read(SMSSENDINTERVALPLUG);
		udpInterval = EEPROM.read(UDPSENDINTERVALPLUG);
		smsPowerProfile = EEPROM.read(SMSPOWERPROFILE);
		udpPowerProfile = EEPROM.read(UDPPOWERPROFILE);
		smsPowerSpeed = EEPROM.read(SMSSPEEDPLUG);
		udpPowerSpeed = EEPROM.read(UDPSPEEDPLUG);
	}
	else
	{
		smsInterval = EEPROM.read(SMSSENDINTERVALBAT);
		udpInterval = EEPROM.read(UDPSENDINTERVALBAT);
		smsPowerProfile = EEPROM.read(SMSPOWERPROFILE) >> 4;
		udpPowerProfile = EEPROM.read(UDPPOWERPROFILE) >> 4;
		smsPowerSpeed = EEPROM.read(SMSSPEEDBAT);
		udpPowerSpeed = EEPROM.read(UDPSPEEDBAT);
	}
}

void d4Interrupt()
{
	d4Switch = 0x01;
}

void d10Interrupt()
{
	d10Switch = 0x01;
}

/*int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}*/

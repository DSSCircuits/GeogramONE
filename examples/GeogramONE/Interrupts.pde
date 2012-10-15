void ringIndicator()
{
	call = 1;
}

void movement()
{
	move = 1;
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

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
	charge |= 0x01;
}

void lowBattery()
{
	battery = 1;
}

void chargerStatus()
{
	delay(2);
	charge = digitalRead(PG_INT);
	charge <<= 1;
}

/*int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}*/

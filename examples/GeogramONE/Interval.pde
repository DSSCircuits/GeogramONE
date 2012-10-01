uint8_t timerMenu()
{
	static unsigned long intervalTimer = millis();
	if((millis() - intervalTimer) >= (timeInterval*1000))
	{
		cmd0 = 0x01;
		intervalTimer = millis();
	}
}



uint8_t timerMenu(uint8_t *cmd0)
{
	static unsigned long intervalTimer = millis();
	if((millis() - intervalTimer) >= (timeInterval*1000))
	{
		*cmd0 = 0x01;
		intervalTimer = millis();
	}
}



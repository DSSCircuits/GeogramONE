uint8_t smsTimerMenu()
{
	static unsigned long smsTimer = millis();
	if((millis() - smsTimer) >= (smsInterval*1000))
	{
		cmd0 = 0x01;
		smsTimer = millis();
	}
}

uint8_t gprsTimerMenu()
{
	static unsigned long gprsTimer = millis();
	if((millis() - gprsTimer) >= (gprsInterval*1000))
	{
		udp |= 0x01;
		gprsTimer = millis();
	}
}

uint8_t smsTimerMenu()
{
	static unsigned long smsTimer = millis();
	if((millis() - smsTimer) >= (smsInterval*1000))
	{
		cmd0 = 0x01;
		smsTimer = millis();
	}
}

uint8_t udpTimerMenu()
{
	static unsigned long udpTimer = millis();
	if((millis() - udpTimer) >= (udpInterval*1000))
	{
		udp |= 0x01;
		udpTimer = millis();
	}
}

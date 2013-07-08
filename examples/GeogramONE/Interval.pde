uint8_t smsTimerMenu()
{
	static unsigned long smsTimer = millis();
	if((millis() - smsTimer) >= (smsInterval*1000))
	{
		if((move & 0x04) && (smsPowerProfile & 0x01))
			cmd0 = 0x01;
		if((lastValid.speed >= smsPowerSpeed) && (smsPowerProfile & 0x02))
			cmd0 = 0x01;
		move &= ~(0x04);
		smsTimer = millis();
	}
}

uint8_t udpTimerMenu()
{
	static unsigned long udpTimer = millis();
	if((millis() - udpTimer) >= (udpInterval*1000))
	{
		if((move & 0x02) && (udpPowerProfile & 0x01))
			udp |= 0x01;
		if((lastValid.speed >= udpPowerSpeed) && (udpPowerProfile & 0x02))
			udp |= 0x01;
		move &= ~(0x02);
		udpTimer = millis();
	}
}
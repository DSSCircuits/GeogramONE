void smsTimerMenu()
{
	static unsigned long smsTimer = millis();
	if((millis() - smsTimer) >= (smsInterval*1000))
	{
		smsPowerProfile &= 0x03;
		if(!smsPowerProfile)
			cmd0 = 0x01;
		else if((move & 0x04) && (smsPowerProfile & 0x01))
		{
			cmd0 = 0x01;
			move &= ~(0x04);
		}
		else if((lastValid.speed >= smsPowerSpeed) && (smsPowerProfile & 0x02))
			cmd0 = 0x01;
		smsTimer = millis();
	}
}

void udpTimerMenu()
{
	static unsigned long udpTimer = millis();
	if((millis() - udpTimer) >= (udpInterval*1000))
	{
		udpPowerProfile &= 0x03;
		if(!udpPowerProfile)
			udp |= 0x01;
		else if((move & 0x02) && (udpPowerProfile & 0x01))
		{
			udp |= 0x01;
			move &= ~(0x02);
		}
		else if((lastValid.speed >= udpPowerSpeed) && (udpPowerProfile & 0x02))
			udp |= 0x01;
		udpTimer = millis();
	}
}


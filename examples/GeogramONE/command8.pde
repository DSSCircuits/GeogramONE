void command8()  //send coordinates every so  to GPRS
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	udpInterval = atol(ptr);
	EEPROM_writeAnything(UDPSENDINTERVALBAT,(unsigned long)udpInterval);
	EEPROM_writeAnything(UDPSENDINTERVALPLUG,(unsigned long)udpInterval);
}

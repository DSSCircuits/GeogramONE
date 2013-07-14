void command4()  //send coordinates every so often
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	smsInterval = atol(ptr);
	EEPROM_writeAnything(SMSSENDINTERVALPLUG,(unsigned long)smsInterval);
	EEPROM_writeAnything(SMSSENDINTERVALBAT,(unsigned long)smsInterval);
}

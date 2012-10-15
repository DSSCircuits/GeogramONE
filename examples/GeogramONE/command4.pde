void command4()  //send coordinates every so often
{
	char *ptr = NULL;
	char *str = NULL;
	ptr = strtok_r(smsData.smsCmdString,".",&str);
	timeInterval = atol(ptr);
	EEPROM_writeAnything(SENDINTERVAL,(unsigned long)timeInterval);
}

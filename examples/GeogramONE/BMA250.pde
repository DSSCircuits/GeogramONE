void BMA250init(uint8_t pinNum, uint16_t tOut)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
	I2c.begin();
	I2c.timeOut(tOut);
	BMA250configureMotion();
	BMA250configureInterrupts();
}

void BMA250configureMotion()
{
	I2c.write(BMA_ADD,(uint8_t)0x11,EEPROM.read(BMA0X11));
	I2c.write(BMA_ADD,(uint8_t)0x25,EEPROM.read(BMA0X25));
	I2c.write(BMA_ADD,(uint8_t)0x26,EEPROM.read(BMA0X26));
	I2c.write(BMA_ADD,(uint8_t)0x0F,EEPROM.read(BMA0X0F));     //GSEL4
	I2c.write(BMA_ADD,(uint8_t)0x10,EEPROM.read(BMA0X10));   //BW7P81
	I2c.write(BMA_ADD,(uint8_t)0x27,EEPROM.read(BMA0X27));  //set slope duration to 1 bits
	I2c.write(BMA_ADD,(uint8_t)0x28,EEPROM.read(BMA0X28));  //set slope threshold to 19.55mg	
}

void BMA250configureInterrupts()
{
	I2c.write(BMA_ADD,(uint8_t)0x11,EEPROM.read(BMA0X11));
	I2c.write(BMA_ADD,(uint8_t)0x25,EEPROM.read(BMA0X25));
	I2c.write(BMA_ADD,(uint8_t)0x26,EEPROM.read(BMA0X26));
	I2c.write(BMA_ADD,(uint8_t)0x19,EEPROM.read(BMA0X19));  	//map to INT1
	I2c.write(BMA_ADD,(uint8_t)0x1A,EEPROM.read(BMA0X1A));     	//map to INT1/INT2
	I2c.write(BMA_ADD,(uint8_t)0x1B,EEPROM.read(BMA0X1B));		//map to INT2
	I2c.write(BMA_ADD,(uint8_t)0x20,EEPROM.read(BMA0X20));  	//INT drive signal
}

void BMA250enableInterrupts()
{
	I2c.write(BMA_ADD,(uint8_t)0x21,EEPROM.read(BMA0X21));  	//INT latching 0x8E
	delayMicroseconds(600);
	I2c.write(BMA_ADD,(uint8_t)0x16,EEPROM.read(BMA0X16));  	//enable INT1 0x07
	I2c.write(BMA_ADD,(uint8_t)0x17,EEPROM.read(BMA0X17));  //enable INT1
}

void BMA250disableInterrupts()
{
	I2c.write(BMA_ADD,(uint8_t)0x16,(uint8_t)0x00);  	//disable Interrupts
	I2c.write(BMA_ADD,(uint8_t)0x17,(uint8_t)0x00);  	//disable Interrupts
}



#include <EEPROM.h>
#include "eepromAnything.h"

/*******EEPROM ADDRESSES**********/
#define PINCODE         		        0
#define SMSADDRESS				5
#define RETURNADDCONFIG			        44
#define TIMEZONE				45   
#define DATEFORMAT				46   
#define ENGMETRIC				47   
#define GEODATAFORMAT1		        	48
#define GEODATAFORMAT2			        50
#define BATTERYLOWLEVEL    		        52
#define SMSSENDINTERVAL			        53
#define IOSTATE0				57
#define IOSTATE1				58
#define IOSTATE2				59
#define IOSTATE3				60
#define IOSTATE4				61
#define IOSTATE5				62
#define IOSTATE6				63
#define IOSTATE7				64
#define IOSTATE8				65
#define IOSTATE9				66
#define SLEEPTIMECONFIG			        68
#define SLEEPTIMEON				69
#define SLEEPTIMEOFF		        	73
#define SPEEDLIMIT				77
#define SPEEDHYST				79
#define ACTIVE1					80
#define INOUT1					81
#define RADIUS1					82
#define LATITUDE1				86
#define LONGITUDE1				90
#define ACTIVE2					94
#define INOUT2					95
#define RADIUS2					96
#define LATITUDE2				100
#define LONGITUDE2				104
#define ACTIVE3					108
#define INOUT3					109
#define RADIUS3					110
#define LATITUDE3				114
#define LONGITUDE3				118
#define BREACHSPEED				122
#define BREACHREPS				123
#define BMA0X0F					124
#define BMA0X10					125
#define BMA0X11					126
#define BMA0X16					127
#define BMA0X17					128
#define BMA0X19					129
#define BMA0X1A					130
#define BMA0X1B					131
#define BMA0X20					132
#define BMA0X21					133
#define BMA0X25					134
#define BMA0X26					135
#define BMA0X27					136
#define BMA0X28					137
#define UDPSENDINTERVAL			        138
#define MOTIONMSG				200
#define BATTERYMSG				225
#define FENCE1MSG				250
#define FENCE2MSG				275
#define FENCE3MSG				300
#define SPEEDMSG				325
#define MAXSPEEDMSG				350
#define GEOGRAMONEID			        375
#define D4MSG					400
#define D10MSG					425
#define HTTP1					450
#define HTTP2					500
#define HTTP3					550
#define IMEI					600
#define GPRS_APN				616
#define GPRS_USER				666
#define GPRS_PASS				691
#define GPRS_HOST				716
#define GPRS_PORT				732
#define UDP_HEADER				734
#define UDP_REPLY				745





char pincode[5]; //pincode must be 4 digits
char smsaddress[39]; //smsaddress must be 38 characters or less
char batteryMsg[25];
char motionMsg[25];
char fence1Msg[25];
char fence2Msg[25];
char fence3Msg[25];
char speedMsg[25];
char geoIDMsg[25];
char maxSpeedMsg[25];
char http1[100];
char http2[100];
char http3[100];
char d4msg[25];
char d10msg[25];
char imei[16];
char gprsApn[50];
char gprsUser[25];
char gprsPass[25];
char gprsHost[16];
char udpHeader[11];
char udpReply[11];

void setup()
{
	Serial.begin(9600);
	delay(2000);
	Serial.flush();
	while(!Serial.available()){}
	Serial.println("Please Wait While EEPROM is Read");
	Serial.println("Register#       Contents");
	Serial.println("-------------------------");
	
	Serial.print(PINCODE);Serial.print("        ");
	EEPROM_readAnything(PINCODE,pincode);Serial.println(pincode);
	
	Serial.print(SMSADDRESS);Serial.print("        ");
	EEPROM_readAnything(SMSADDRESS,smsaddress);Serial.println(smsaddress);
	
	uint8_t abyte;
	Serial.print(RETURNADDCONFIG,DEC);Serial.print("        ");
	EEPROM_readAnything(RETURNADDCONFIG,abyte);Serial.println(abyte,DEC);

	int8_t sbyte;
	Serial.print(TIMEZONE,DEC);Serial.print("        ");
	EEPROM_readAnything(TIMEZONE,sbyte);Serial.println(sbyte,DEC);
	
	Serial.print(DATEFORMAT,DEC);Serial.print("        ");
	EEPROM_readAnything(DATEFORMAT,abyte);Serial.println(abyte,DEC);
	
	Serial.print(ENGMETRIC,DEC);Serial.print("        ");
	EEPROM_readAnything(ENGMETRIC,abyte);Serial.println(abyte,DEC);

	unsigned int ninteger;
	Serial.print(GEODATAFORMAT1,DEC);Serial.print("        ");
	EEPROM_readAnything(GEODATAFORMAT1,ninteger);Serial.println(ninteger,DEC);
	
	Serial.print(GEODATAFORMAT2,DEC);Serial.print("        ");
	EEPROM_readAnything(GEODATAFORMAT2,ninteger);Serial.println(ninteger,DEC);
	
	Serial.print(BATTERYLOWLEVEL,DEC);Serial.print("        ");
	EEPROM_readAnything(BATTERYLOWLEVEL,abyte);Serial.println(abyte,DEC);

	unsigned long ulong;
	Serial.print(SMSSENDINTERVAL,DEC);Serial.print("        ");
	EEPROM_readAnything(SMSSENDINTERVAL,ulong);Serial.println(ulong,DEC);
	
	Serial.print(IOSTATE0,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE0,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE1,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE1,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE2,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE2,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE3,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE3,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE4,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE4,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE5,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE5,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE6,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE6,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE7,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE7,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE8,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE8,abyte);Serial.println(abyte,DEC);
	
	Serial.print(IOSTATE9,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE9,abyte);Serial.println(abyte,DEC);

	Serial.print(SLEEPTIMECONFIG,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMECONFIG,abyte);Serial.println(abyte,DEC);

	Serial.print(SLEEPTIMEON,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMEON,ulong);Serial.println(ulong,DEC);
	
	Serial.print(SLEEPTIMEOFF,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMEOFF,ulong);Serial.println(ulong,DEC);
	
	Serial.print(SPEEDLIMIT,DEC);Serial.print("        ");
	EEPROM_readAnything(SPEEDLIMIT,ninteger);Serial.println(ninteger,DEC);
	
	Serial.print(SPEEDHYST,DEC);Serial.print("        ");
	EEPROM_readAnything(SPEEDHYST,abyte);Serial.println(abyte,DEC);

	Serial.print(ACTIVE1,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE1,abyte);Serial.println(abyte,DEC);

	Serial.print(INOUT1,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT1,abyte);Serial.println(abyte,DEC);

	Serial.print(RADIUS1,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS1,ulong);Serial.println(ulong,DEC);

	int32_t slong;
	Serial.print(LATITUDE1,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE1,slong);Serial.println(slong,DEC);

	Serial.print(LONGITUDE1,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE1,slong);Serial.println(slong,DEC);

	Serial.print(ACTIVE2,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE2,abyte);Serial.println(abyte,DEC);

	Serial.print(INOUT2,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT2,abyte);Serial.println(abyte,DEC);

	Serial.print(RADIUS2,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS2,ulong);Serial.println(ulong,DEC);

	Serial.print(LATITUDE2,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE2,slong);Serial.println(slong,DEC);

	Serial.print(LONGITUDE2,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE2,slong);Serial.println(slong,DEC);
	
	Serial.print(ACTIVE3,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE3,abyte);Serial.println(abyte,DEC);
	
	Serial.print(INOUT3,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT3,abyte);Serial.println(abyte,DEC);
	
	Serial.print(RADIUS3,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS3,ulong);Serial.println(ulong,DEC);
	
	Serial.print(LATITUDE3,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE3,slong);Serial.println(slong,DEC);
	
	Serial.print(LONGITUDE3,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE3,slong);Serial.println(slong,DEC);
	
	Serial.print(BREACHSPEED,DEC);Serial.print("        ");
	EEPROM_readAnything(BREACHSPEED,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BREACHREPS,DEC);Serial.print("        ");
	EEPROM_readAnything(BREACHREPS,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X0F,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X0F,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X10,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X10,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X11,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X11,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X16,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X16,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X17,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X17,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X19,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X19,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X1A,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X1A,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X1B,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X1B,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X20,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X20,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X21,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X21,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X25,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X25,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X26,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X26,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X27,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X27,abyte);Serial.println(abyte,DEC);
	
	Serial.print(BMA0X28,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X28,abyte);Serial.println(abyte,DEC);
	
	EEPROM_writeAnything(UDPSENDINTERVAL,(unsigned long)ulong);
	Serial.print(UDPSENDINTERVAL,DEC);Serial.print("        ");
	EEPROM_readAnything(UDPSENDINTERVAL,ulong);Serial.println(ulong,DEC);
	
	Serial.print(MOTIONMSG);Serial.print("        ");
	EEPROM_readAnything(MOTIONMSG,motionMsg);Serial.println(motionMsg);
	
	Serial.print(BATTERYMSG);Serial.print("        ");
	EEPROM_readAnything(BATTERYMSG,batteryMsg);Serial.println(batteryMsg);

	Serial.print(FENCE1MSG);Serial.print("        ");
	EEPROM_readAnything(FENCE1MSG,fence1Msg);Serial.println(fence1Msg);
	
	Serial.print(FENCE2MSG);Serial.print("        ");
	EEPROM_readAnything(FENCE2MSG,fence2Msg);Serial.println(fence2Msg);
	
	Serial.print(FENCE3MSG);Serial.print("        ");
	EEPROM_readAnything(FENCE3MSG,fence3Msg);Serial.println(fence3Msg);

	Serial.print(SPEEDMSG);Serial.print("        ");
	EEPROM_readAnything(SPEEDMSG,speedMsg);Serial.println(speedMsg);

	Serial.print(MAXSPEEDMSG);Serial.print("        ");
	EEPROM_readAnything(MAXSPEEDMSG,maxSpeedMsg);Serial.println(maxSpeedMsg);
	
	Serial.print(GEOGRAMONEID);Serial.print("        ");
	EEPROM_readAnything(GEOGRAMONEID,geoIDMsg);Serial.println(geoIDMsg);

	Serial.print(D4MSG);Serial.print("        ");
	EEPROM_readAnything(D4MSG,d4msg);Serial.println(d4msg);
	
	Serial.print(D10MSG);Serial.print("        ");
	EEPROM_readAnything(D10MSG,d10msg);Serial.println(d10msg);

	Serial.print(HTTP1);Serial.print("        ");
	EEPROM_readAnything(HTTP1,http1);Serial.println(http1);
	
	Serial.print(HTTP2);Serial.print("        ");
	EEPROM_readAnything(HTTP2,http2);Serial.println(http2);
	
	Serial.print(HTTP3);Serial.print("        ");
	EEPROM_readAnything(HTTP3,http3);Serial.println(http3);

	Serial.print(IMEI);Serial.print("        ");
	EEPROM_readAnything(IMEI,imei);Serial.println(imei);

	Serial.print(GPRS_APN);Serial.print("        ");
	EEPROM_readAnything(GPRS_APN,gprsApn);Serial.println(gprsApn);

	Serial.print(GPRS_USER);Serial.print("        ");
	EEPROM_readAnything(GPRS_USER,gprsUser);Serial.println(gprsUser);

	Serial.print(GPRS_PASS);Serial.print("        ");
	EEPROM_readAnything(GPRS_PASS,gprsPass);Serial.println(gprsPass);

	Serial.print(GPRS_HOST);Serial.print("        ");
	EEPROM_readAnything(GPRS_HOST,gprsHost);Serial.println(gprsHost);

	Serial.print(GPRS_PORT,DEC);Serial.print("        ");
	EEPROM_readAnything(GPRS_PORT,ninteger);Serial.println(ninteger,DEC);
	
	Serial.print(UDP_HEADER);Serial.print("        ");
	EEPROM_readAnything(UDP_HEADER,udpHeader);Serial.println(udpHeader);

	Serial.print(UDP_REPLY);Serial.print("        ");
	EEPROM_readAnything(UDP_REPLY,udpReply);Serial.println(udpReply);
	Serial.println();	

	Serial.println("Finished Reading EEPROM");
}

void loop()
{
  
  
}

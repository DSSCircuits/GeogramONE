
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






char pincode[5] = "1234"; //pincode must be 4 digits
char smsaddress[39] = ""; //smsaddress must be 38 characters or less
char batteryMsg[25] = "Low Battery Alert";
char motionMsg[25] = "Motion Detected";
char fence1Msg[25] = "Fence 1 Breach";
char fence2Msg[25] = "Fence 2 Breach";
char fence3Msg[25] = "Fence 3 Breach";
char speedMsg[25] = "Speed Limit Exceeded";
char geoIDMsg[25] = "GO1";
char maxSpeedMsg[25] = "Max Speed = ";
char http1[100] = "http://maps.google.com/maps?q=";
char http2[100] = "+(";
char http3[100] = ")&z=19";
char d4msg[25] = "D4 Switch Alert";
char d10msg[25] = "D10 Switch Alert";
char imei[16] = ""; //15 digit number on GSM chip
char gprsApn[50] = "wholesale"; //SIM card specific APN.  wholesale is used on Platinumtel
char gprsUser[25] = "";
char gprsPass[25] = "";
char gprsHost[16] = "193.193.165.166"; //Server address for GPS-Trace Orange
char gprsHeader[11] = "#SD#";
char gprsReply[11] = "#ASD#1";

void setup()
{
	Serial.begin(9600);
	delay(2000);
        Serial.flush();
        Serial.println("PRESS ENTER TO PROGRAM EEPROM");
        while(!Serial.available()){}
	Serial.println("Please Wait While EEPROM is Written");
	Serial.println("Written         Read Back");
	Serial.println("-------------------------");
	
	EEPROM_writeAnything(PINCODE,pincode);
	Serial.print(pincode);Serial.print("        ");
	EEPROM_readAnything(PINCODE,pincode);Serial.println(pincode);
	
	EEPROM_writeAnything(SMSADDRESS,smsaddress);
	Serial.print(smsaddress);Serial.print("        ");
	EEPROM_readAnything(SMSADDRESS,smsaddress);Serial.println(smsaddress);
		
	uint8_t abyte = 0;
	EEPROM_writeAnything(RETURNADDCONFIG,(uint8_t)abyte); //changed to a 1 from 0
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(RETURNADDCONFIG,abyte);Serial.println(abyte,DEC);

	int8_t sbyte = 0;
	EEPROM_writeAnything(TIMEZONE,(int8_t)sbyte);   //use -4 for EST
	Serial.print(sbyte,DEC);Serial.print("        ");
	EEPROM_readAnything(TIMEZONE,sbyte);Serial.println(sbyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(DATEFORMAT,(uint8_t)abyte);   // 0 - mm/dd/yy, 1 - yy/mm/dd
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(DATEFORMAT,abyte);Serial.println(abyte,DEC);

	abyte = 0;
	EEPROM_writeAnything(ENGMETRIC,(uint8_t)abyte);  // 0 - English (mph, ft, etc...), 1 = Metric (kph, m, etc...)
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(ENGMETRIC,abyte);Serial.println(abyte,DEC);

	unsigned int ninteger = 3;
	EEPROM_writeAnything(GEODATAFORMAT1,(uint16_t)ninteger); // 0x0003 date and time
	Serial.print(ninteger,DEC);Serial.print("        ");
	EEPROM_readAnything(GEODATAFORMAT1,ninteger);Serial.println(ninteger,DEC);
	
	ninteger = 0x4818;
	EEPROM_writeAnything(GEODATAFORMAT2,(uint16_t)ninteger); //Speed, course, battery percent, ID
	Serial.print(ninteger,DEC);Serial.print("        ");
	EEPROM_readAnything(GEODATAFORMAT2,ninteger);Serial.println(ninteger,DEC);
	
	abyte = 32;
	EEPROM_writeAnything(BATTERYLOWLEVEL,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BATTERYLOWLEVEL,abyte);Serial.println(abyte,DEC);

	unsigned long ulong = 0;
	EEPROM_writeAnything(SMSSENDINTERVAL,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(SMSSENDINTERVAL,ulong);Serial.println(ulong,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(IOSTATE0,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE0,abyte);Serial.println(abyte,DEC);
	
	abyte = 6;
	EEPROM_writeAnything(IOSTATE1,(uint8_t)abyte); //int falling
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE1,abyte);Serial.println(abyte,DEC);
	
	abyte = 4;
	EEPROM_writeAnything(IOSTATE2,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE2,abyte);Serial.println(abyte,DEC);
	
	abyte = 4;
	EEPROM_writeAnything(IOSTATE3,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE3,abyte);Serial.println(abyte,DEC);
	
	abyte = 4;
	EEPROM_writeAnything(IOSTATE4,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE4,abyte);Serial.println(abyte,DEC);
	
	abyte = 4;
	EEPROM_writeAnything(IOSTATE5,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE5,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(IOSTATE6,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE6,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(IOSTATE7,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE7,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(IOSTATE8,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE8,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(IOSTATE9,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(IOSTATE9,abyte);Serial.println(abyte,DEC);

	abyte = 3;
	EEPROM_writeAnything(SLEEPTIMECONFIG,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMECONFIG,abyte);Serial.println(abyte,DEC);

	ulong = 0;
	EEPROM_writeAnything(SLEEPTIMEON,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMEON,ulong);Serial.println(ulong,DEC);
	
	ulong = 0;
	EEPROM_writeAnything(SLEEPTIMEOFF,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(SLEEPTIMEOFF,ulong);Serial.println(ulong,DEC);
	
	ninteger = 0;
	EEPROM_writeAnything(SPEEDLIMIT,(uint16_t)ninteger);
	Serial.print(ninteger,DEC);Serial.print("        ");
	EEPROM_readAnything(SPEEDLIMIT,ninteger);Serial.println(ninteger,DEC);
	
	abyte = 3;
	EEPROM_writeAnything(SPEEDHYST,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(SPEEDHYST,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(ACTIVE1,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE1,abyte);Serial.println(abyte,DEC);

	abyte = 0;
	EEPROM_writeAnything(INOUT1,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT1,abyte);Serial.println(abyte,DEC);

	ulong = 0;
	EEPROM_writeAnything(RADIUS1,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS1,ulong);Serial.println(ulong,DEC);

	int32_t slong = 0;
	EEPROM_writeAnything(LATITUDE1,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE1,slong);Serial.println(slong,DEC);

	slong = 0;
	EEPROM_writeAnything(LONGITUDE1,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE1,slong);Serial.println(slong,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(ACTIVE2,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE2,abyte);Serial.println(abyte,DEC);

	abyte = 0;
	EEPROM_writeAnything(INOUT2,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT2,abyte);Serial.println(abyte,DEC);

	ulong = 0;
	EEPROM_writeAnything(RADIUS2,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS2,ulong);Serial.println(ulong,DEC);

	slong = 0;
	EEPROM_writeAnything(LATITUDE2,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE2,slong);Serial.println(slong,DEC);

	slong = 0;
	EEPROM_writeAnything(LONGITUDE2,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE2,slong);Serial.println(slong,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(ACTIVE3,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(ACTIVE3,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(INOUT3,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(INOUT3,abyte);Serial.println(abyte,DEC);
	
	ulong = 0;
	EEPROM_writeAnything(RADIUS3,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(RADIUS3,ulong);Serial.println(ulong,DEC);
	
	slong = 0;
	EEPROM_writeAnything(LATITUDE3,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LATITUDE3,slong);Serial.println(slong,DEC);
	
	slong = 0;
	EEPROM_writeAnything(LONGITUDE3,(long)slong);
	Serial.print(slong,DEC);Serial.print("        ");
	EEPROM_readAnything(LONGITUDE3,slong);Serial.println(slong,DEC);
	
	abyte = 2;
	EEPROM_writeAnything(BREACHSPEED,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BREACHSPEED,abyte);Serial.println(abyte,DEC);
	
	abyte = 0x0A;
	EEPROM_writeAnything(BREACHREPS,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BREACHREPS,abyte);Serial.println(abyte,DEC);
	
	abyte = 5;
	EEPROM_writeAnything(BMA0X0F,(uint8_t)abyte); //was 3
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X0F,abyte);Serial.println(abyte,DEC);
	
	abyte = 8;
	EEPROM_writeAnything(BMA0X10,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X10,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(BMA0X11,(uint8_t)abyte); //default 0x00 per datasheet
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X11,abyte);Serial.println(abyte,DEC);
	
	abyte = 7;
	EEPROM_writeAnything(BMA0X16,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X16,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(BMA0X17,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X17,abyte);Serial.println(abyte,DEC);
	
	abyte = 4;
	EEPROM_writeAnything(BMA0X19,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X19,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(BMA0X1A,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X1A,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(BMA0X1B,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X1B,abyte);Serial.println(abyte,DEC);
	
	abyte = 6;
	EEPROM_writeAnything(BMA0X20,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X20,abyte);Serial.println(abyte,DEC);
	
	abyte = 0x8E;
	EEPROM_writeAnything(BMA0X21,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X21,abyte);Serial.println(abyte,DEC);
	
	abyte = 0x0F;
	EEPROM_writeAnything(BMA0X25,(uint8_t)abyte); //default 0x0F per datasheet
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X25,abyte);Serial.println(abyte,DEC);
	
	abyte = 0xC0;
	EEPROM_writeAnything(BMA0X26,(uint8_t)abyte); //default 0xC0 per datasheet
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X26,abyte);Serial.println(abyte,DEC);
	
	abyte = 0;
	EEPROM_writeAnything(BMA0X27,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X27,abyte);Serial.println(abyte,DEC);
	
	abyte = 5;
	EEPROM_writeAnything(BMA0X28,(uint8_t)abyte);
	Serial.print(abyte,DEC);Serial.print("        ");
	EEPROM_readAnything(BMA0X28,abyte);Serial.println(abyte,DEC);
	
        ulong = 0;
	EEPROM_writeAnything(UDPSENDINTERVAL,(unsigned long)ulong);
	Serial.print(ulong,DEC);Serial.print("        ");
	EEPROM_readAnything(UDPSENDINTERVAL,ulong);Serial.println(ulong,DEC);
	
	EEPROM_writeAnything(MOTIONMSG,motionMsg);
	Serial.print(motionMsg);Serial.print("        ");
	EEPROM_readAnything(MOTIONMSG,motionMsg);Serial.println(motionMsg);
	
	EEPROM_writeAnything(BATTERYMSG,batteryMsg);
	Serial.print(batteryMsg);Serial.print("        ");
	EEPROM_readAnything(BATTERYMSG,batteryMsg);Serial.println(batteryMsg);
	
	EEPROM_writeAnything(FENCE1MSG,fence1Msg);
	Serial.print(fence1Msg);Serial.print("        ");
	EEPROM_readAnything(FENCE1MSG,fence1Msg);Serial.println(fence1Msg);
	
	EEPROM_writeAnything(FENCE2MSG,fence2Msg);
	Serial.print(fence2Msg);Serial.print("        ");
	EEPROM_readAnything(FENCE2MSG,fence2Msg);Serial.println(fence2Msg);
	
	EEPROM_writeAnything(FENCE3MSG,fence3Msg);
	Serial.print(fence3Msg);Serial.print("        ");
	EEPROM_readAnything(FENCE3MSG,fence3Msg);Serial.println(fence3Msg);
	
	EEPROM_writeAnything(SPEEDMSG,speedMsg);
	Serial.print(speedMsg);Serial.print("        ");
	EEPROM_readAnything(SPEEDMSG,speedMsg);Serial.println(speedMsg);
	
	EEPROM_writeAnything(MAXSPEEDMSG,maxSpeedMsg);
	Serial.print(maxSpeedMsg);Serial.print("        ");
	EEPROM_readAnything(MAXSPEEDMSG,maxSpeedMsg);Serial.println(maxSpeedMsg);
	
	EEPROM_writeAnything(GEOGRAMONEID,geoIDMsg);
	Serial.print(geoIDMsg);Serial.print("        ");
	EEPROM_readAnything(GEOGRAMONEID,geoIDMsg);Serial.println(geoIDMsg);

	EEPROM_writeAnything(D4MSG,d4msg);
	Serial.print(d4msg);Serial.print("        ");
	EEPROM_readAnything(D4MSG,d4msg);Serial.println(d4msg);
	
	EEPROM_writeAnything(D10MSG,d10msg);
	Serial.print(d10msg);Serial.print("        ");
	EEPROM_readAnything(D10MSG,d10msg);Serial.println(d10msg);
	
	EEPROM_writeAnything(HTTP1,http1);
	Serial.print(http1);Serial.print("        ");
	EEPROM_readAnything(HTTP1,http1);Serial.println(http1);
	
	EEPROM_writeAnything(HTTP2,http2);
	Serial.print(http2);Serial.print("        ");
	EEPROM_readAnything(HTTP2,http2);Serial.println(http2);
	
	EEPROM_writeAnything(HTTP3,http3);
	Serial.print(http3);Serial.print("        ");
	EEPROM_readAnything(HTTP3,http3);Serial.println(http3);

	EEPROM_writeAnything(IMEI,imei);
	Serial.print(imei);Serial.print("        ");
	EEPROM_readAnything(IMEI,imei);Serial.println(imei);

 	EEPROM_writeAnything(GPRS_APN,gprsApn);
	Serial.print(gprsApn);Serial.print("        ");
	EEPROM_readAnything(GPRS_APN,gprsApn);Serial.println(gprsApn);

 	EEPROM_writeAnything(GPRS_USER,gprsUser);
	Serial.print(gprsUser);Serial.print("        ");
	EEPROM_readAnything(GPRS_USER,gprsUser);Serial.println(gprsUser);

 	EEPROM_writeAnything(GPRS_PASS,gprsPass);
	Serial.print(gprsPass);Serial.print("        ");
	EEPROM_readAnything(GPRS_PASS,gprsPass);Serial.println(gprsPass);

 	EEPROM_writeAnything(GPRS_HOST,gprsHost);
	Serial.print(gprsHost);Serial.print("        ");
	EEPROM_readAnything(GPRS_HOST,gprsHost);Serial.println(gprsHost);

	ninteger = 20332;
	EEPROM_writeAnything(GPRS_PORT,(uint16_t)ninteger); //GPRS port number
	Serial.print(ninteger,DEC);Serial.print("        ");
	EEPROM_readAnything(GPRS_PORT,ninteger);Serial.println(ninteger,DEC);

 	EEPROM_writeAnything(UDP_HEADER,gprsHeader);
	Serial.print(gprsHeader);Serial.print("        ");
	EEPROM_readAnything(UDP_HEADER,gprsHeader);Serial.println(gprsHeader);

 	EEPROM_writeAnything(UDP_REPLY,gprsReply);
	Serial.print(gprsReply);Serial.print("        ");
	EEPROM_readAnything(UDP_REPLY,gprsReply);Serial.println(gprsReply);
	Serial.println();	

	Serial.println("Finished Writing EEPROM");
}

void loop()
{
}

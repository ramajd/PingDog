#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Ethernet.h>

#include "ICMPPing.h"

unsigned char mac[6] = { 0xec, 0xa8, 0x00, 0x00, 0x00, 0x00 };

IPAddress ip1 = { 0, 0, 0, 0 };
IPAddress ip2 = { 0, 0, 0, 0 };
IPAddress ip3 = { 0, 0, 0, 0 };
IPAddress ip4 = { 0, 0, 0, 0 };

IPAddress* ips[4] { &ip1, &ip2, &ip3, &ip4 };
uint8_t cnts[4] = { 0, 0, 0, 0 };
uint8_t pins[4] = { 5, 6, 7, 8 };

IPAddress myIp = { 0, 0, 0, 0 };
char ip_mode = 's';

int cycleTime = 60;
int timeoutCount = 5;
int toggleDuration = 200;
unsigned long lastCycle = 0;
unsigned long curTime = 0;

SOCKET pingSocket = 0;
ICMPPing ping(pingSocket, (uint16_t) random(0, 255));

String ipstr(IPAddress ip) {
	char ipstr[16] = { 0x00 };
	sprintf(ipstr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	return String(ipstr);
}

bool read_conf(String line, String key, String& val) {
	line.trim();
	key += "=";
	if (line.startsWith(key)) {
		line.replace(key, "");
		val = line;
		return true;
	}
	return false;
}

bool read_conf(String line, String key, int& val) {

	String str;
	bool ret = read_conf(line, key, str);
	if (ret) {
		val = str.toInt();
		return true;
	}
	return false;
}
bool read_conf(String line, String key, char& val)
{
	String str;
	bool ret = read_conf(line, key, str);
	if (ret) {
		val = str[0];
		return true;
	}
	return false;
}
bool read_conf(String line, String key, IPAddress& val)
{
	String str;
	bool ret = read_conf(line, key, str);
	if (ret) {
		val.fromString(str);
		return true;
	}
	return false;
}

void setup() {

	Serial.begin(9600);
	while (!Serial)
		;

	Serial.println("PingDog(V1.0) @minertools");
	Serial.println("---");

	if (!SD.begin(9600, 4))
	{
		Serial.println("!SD");
		for (;;)
			;
	}

	//read configuration:
	File f = SD.open("config.ini");
	if (f)
	{
		while (f.available())
		{
			String buffer = f.readStringUntil('\n');
			read_conf(buffer, "cyc", cycleTime);
			read_conf(buffer, "cnt", timeoutCount);
			read_conf(buffer, "dur", toggleDuration);
			read_conf(buffer, "mod", ip_mode);
			read_conf(buffer, "ips", myIp);
			read_conf(buffer, "ip1", ip1);
			read_conf(buffer, "ip2", ip2);
			read_conf(buffer, "ip3", ip3);
			read_conf(buffer, "ip4", ip4);
		}
	}
	Serial.println(String("cyc=") + cycleTime);
	Serial.println(String("cnt=") + timeoutCount);
	Serial.println(String("dur=") + toggleDuration);
	Serial.println(String("mod=") + String(ip_mode));
	Serial.println(String("ips=") + ipstr(myIp));
	Serial.println(String("ip1=") + ipstr(ip1));
	Serial.println(String("ip2=") + ipstr(ip2));
	Serial.println(String("ip3=") + ipstr(ip3));
	Serial.println(String("ip4=") + ipstr(ip4));

	//initiating MAC address
	if (EEPROM.read(1) == '#')
	{
		for (int i = 2; i < 6; i++)
			mac[i] = EEPROM.read(i);
	}
//	if ((ip_mode == 'd') || (ip_mode == 'D'))
//	{
//		if (Ethernet.begin(mac) == 0)
//			Serial.println("!DHCP");
//	}
//	else
//	{
		Ethernet.begin(mac, myIp);
//	}

	for (int i=0; i < 4; i++)
		pinMode(pins[i], OUTPUT);
	pinMode(9, OUTPUT);

	Serial.println("-> Module started.");
}

void toggle(uint8_t pin, uint32_t t) {
	digitalWrite(pin, HIGH);
	delay(t);
	digitalWrite(pin, LOW);
	delay(t);
}

void loop() {

	toggle(9, 500);

	curTime = millis();
	if (((curTime - lastCycle) / 1000) > cycleTime)
	{
		lastCycle = curTime;

		for (int i = 0; i < 4; i++)
		{
			Serial.print(String(i) + ": " + ipstr(*ips[i]));
			if ((*ips[i])[0] == 0)
			{
				Serial.println(": D");
			}
			else
			{
				ICMPEchoReply echoReply = ping(*ips[i], 4);
				if (echoReply.status == SUCCESS)
				{
					cnts[i] = 0;
					Serial.println(String(": S"));
				}
				else
				{
					cnts[i]++;
					Serial.println(String(": F (") + String(cnts[i]) + String(")"));
					if (cnts[i] >= timeoutCount) {
						toggle(pins[i], toggleDuration);
						cnts[i] = 0;
					}
				}
			}
		}
	}
}

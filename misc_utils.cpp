/*
 * misc_utils.cpp
 *
 *  Created on: Farvardin 28, 1397 AP
 *      Author: reza
 */

#include "misc_utils.h"
#include <EEPROM.h>
#include "IniFile.h"

#define CONFIG_SECTION_NETWORK "network"
#define CONFIG_SECTION_GENERAL "general"
#define CONFIG_DEST_IP1   "DESTIP1"
#define CONFIG_DEST_IP2   "DESTIP2"
#define CONFIG_DEST_IP3   "DESTIP3"
#define CONFIG_DEST_IP4   "DESTIP4"

#define CONFIG_SELF_IP   "SELFIP"
#define CONFIG_ETH_MODE  "MODE"

#define CONFIG_CYCLE "CYCLE"

#define CONFIG_BUFFER_LEN 30

bool init_serial(uint32_t baudrate) {
	Serial.begin(baudrate);
	while (!Serial) {
	}
	return true;
}

bool init_sd_card(uint8_t chip, uint32_t baudrate) {
	if (!SD.begin(baudrate, chip)) {
		return false;
	}
	return true;
}

bool init_mac_address(unsigned char* mac) {
	if (EEPROM.read(1) == '#') {
		for (int i = 2; i < 6; i++) {
			mac[i] = EEPROM.read(i);
		}
	} else {
		randomSeed(analogRead(0));
		for (int i = 2; i < 6; i++) {
			mac[i] = random(0, 255);
			EEPROM.write(i, mac[i]);
		}
		EEPROM.write(1, '#');
	}
	return true;
}

bool init_ethernet(const app_config_t& cfg, unsigned char* mac) {
	if (cfg.eth_mode == ETH_MODE_DHCP) {
		if (Ethernet.begin(mac) == 0) {
			return false;
		}
	} else {			//static ip
		Ethernet.begin(mac, cfg.self_ip);
	}
	return true;
}

bool read_config(const char* cfg_name, app_config_t& cfg) {
	IniFile cfgFile(cfg_name);
	if (cfgFile.open()) {
		char buffer[CONFIG_BUFFER_LEN] = { 0x00 };
		cfgFile.getIPAddress(CONFIG_SECTION_NETWORK, CONFIG_DEST_IP1, buffer, CONFIG_BUFFER_LEN, cfg.dest_ip1);
		cfgFile.getIPAddress(CONFIG_SECTION_NETWORK, CONFIG_DEST_IP2, buffer, CONFIG_BUFFER_LEN, cfg.dest_ip2);
		cfgFile.getIPAddress(CONFIG_SECTION_NETWORK, CONFIG_DEST_IP3, buffer, CONFIG_BUFFER_LEN, cfg.dest_ip3);
		cfgFile.getIPAddress(CONFIG_SECTION_NETWORK, CONFIG_DEST_IP4, buffer, CONFIG_BUFFER_LEN, cfg.dest_ip4);

		cfgFile.getValue(CONFIG_SECTION_NETWORK, CONFIG_ETH_MODE, buffer, CONFIG_BUFFER_LEN);
		cfg.eth_mode = (strcmp(buffer, "dhcp") == 0) ? ETH_MODE_DHCP : ETH_MODE_STATIC;
		cfgFile.getIPAddress(CONFIG_SECTION_NETWORK, CONFIG_SELF_IP, buffer, CONFIG_BUFFER_LEN, cfg.self_ip);

		cfgFile.getValue(CONFIG_SECTION_GENERAL, CONFIG_CYCLE, buffer, CONFIG_BUFFER_LEN, cfg.cycle_sec);
		return true;
	}
	return false;
}

String ip_to_string(const IPAddress& ip) {
	char ipstr[16] = { 0x00 };
	sprintf(ipstr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	return String(ipstr);
//	return String(ip[0]) + String(".") + String(ip[1]) + String(".") + String(ip[2]) + String(".") + String(ip[3]);
}

String mac_to_string(const unsigned char* mac) {
	char mac_str[18];
	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6]);
	return String(mac_str);
}

void print_error(const char* err)
{
	Serial.println(String("Error: ") + String(err));
}

void print_conf(const char* key, int val)
{
	Serial.println(String(key) + String(": ") + val);
}
void print_conf(const char* key, const char* val)
{
	Serial.println(String(key) + String(": ") + val);
}


//void print_info(const char* frmt, ...)
//{
//	va_list args;
//
//}

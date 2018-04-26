/*
 * misc_utils.h
 *
 *  Created on: Farvardin 28, 1397 AP
 *      Author: reza
 */

#ifndef MISC_UTILS_H_
#define MISC_UTILS_H_

#include <Arduino.h>
#include <SD.h>
#include <Ethernet.h>
#include <EEPROM.h>

#define CONFIG_BUFFER_LEN "/config.ini"

#define ETH_MODE_DHCP   1
#define ETH_MODE_STATIC 2

typedef struct _app_config_t {
	IPAddress dest_ip1;
	IPAddress dest_ip2;
	IPAddress dest_ip3;
	IPAddress dest_ip4;

	IPAddress self_ip;
	byte eth_mode;

	int32_t cycle_sec;
} app_config_t;

bool init_serial(uint32_t baudrate = 9600);
bool init_sd_card(uint8_t chip, uint32_t baudrate = 9600);
bool init_mac_address(unsigned char* mac);
bool init_ethernet(const app_config_t& cfg, unsigned char* mac);

bool read_config(const char* cfg_name, app_config_t& cfg);

String ip_to_string(const IPAddress&  ip);
String mac_to_string(const unsigned char* mac);

void print_error(const char* err);
void print_conf(const char* key, int val);
void print_conf(const char* key, const char* val);


#endif /* MISC_UTILS_H_ */

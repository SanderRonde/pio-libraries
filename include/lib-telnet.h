#pragma once

#include <WiFiClient.h>

#define LOG Telnet::client.print
#define LOGN Telnet::client.println
#define LOGLN LOGN
#define LOGF Telnet::client.printf

namespace Telnet
{
	extern WiFiClient client;

	void setup(const char *name, const char *net_ssid, const char *net_pw, const char *telnet_ip, int telnet_port);
	void setup(const char *name, const char *net_ssid, const char *net_pw);

	void loop(const char *telnet_ip, int telnet_port);
	void loop();
}
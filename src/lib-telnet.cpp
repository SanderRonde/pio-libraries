#include <WiFiClient.h>
#include <Arduino.h>
#include <string.h>
#include <lib-telnet.h>
#include <lib-net.h>
#include <lib-includes.h>

#define DEFAULT_TELNET_IP "192.168.1.3"
#define DEFAULT_TELNET_PORT 1500

#define RETRY_TIME 30

namespace Telnet
{
	WiFiClient client;

	char *_name = (char *)malloc(sizeof(char) * 50);
	void connect(const char *telnet_ip, int telnet_port)
	{
		client.connect(telnet_ip, telnet_port);
		client.write("set-name:");
		client.write(_name);
		client.write("\n");
		client.write("Hi\n");
#ifdef ESP8266
		client.write(("Reset reason: " + ESP.getResetReason() + "\n").c_str());
#endif
	}

	void setup(const char *name, const char *net_ssid, const char *net_pw, const char *telnet_ip, int telnet_port)
	{
		strcpy(_name, name);

		Net::setup(net_ssid, net_pw);
		Net::await_wifi();

		connect(telnet_ip, telnet_port);
	}

	void setup(const char *name, const char *net_ssid, const char *net_pw)
	{
		setup(name, net_ssid, net_pw, DEFAULT_TELNET_IP, DEFAULT_TELNET_PORT);
	}

	unsigned long last_connect = millis();
	void loop(const char *telnet_ip, int telnet_port)
	{
		if (millis() - last_connect > RETRY_TIME * 1000)
		{
#ifdef ESP8266
			client.keepAlive();
#endif
			if (!client.connected())
			{
				connect(telnet_ip, telnet_port);
			}
		}
	}

	void loop()
	{
		loop(DEFAULT_TELNET_IP, DEFAULT_TELNET_PORT);
	}
}
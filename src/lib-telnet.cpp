#include <WiFiClient.h>
#include <Arduino.h>
#include <string.h>
#include <lib-telnet.h>
#include <lib-net.h>
#include <lib-includes.h>

#define RETRY_TIME 30

namespace Telnet
{
	WiFiClient client;
	const char* _telnet_ip;
	int _telnet_port;

	char *_name = (char *)malloc(sizeof(char) * 50);
	void connect(const char *telnet_ip, int telnet_port)
	{
		_telnet_ip = telnet_ip;
		_telnet_port = telnet_port;
		client.connect(telnet_ip, telnet_port);
		client.write("set-name:");
		client.write(_name);
		client.write("\n");
		client.write("set-ip:");
		client.write(Net::ipToString(WiFi.localIP()).c_str());
		client.write("\n");
		client.write("Hi\n");
#ifdef ESP8266
		client.write(("Reset reason: " + ESP.getResetReason() + "\n").c_str());
#endif
	}

	void setup(const char *name, const char *telnet_ip, int telnet_port)
	{
		strcpy(_name, name);

		Net::await_wifi();

		connect(telnet_ip, telnet_port);
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
		loop(_telnet_ip, _telnet_port);
	}
}
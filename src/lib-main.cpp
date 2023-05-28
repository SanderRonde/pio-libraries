#include <lib-telnet.h>
#include <lib-net.h>
#include <lib-main.h>
#include <lib-ota.h>
#include <lib-sws.h>
#include "Arduino.h"

namespace Main
{
	void connect_pre()
	{
		Serial.begin(115200);
		Serial.println("Booting");
	}

	void add_ap(const char *ssid, const char *pw)
	{
		wifi.addAP(ssid, pw);
	}

	void connect_post(const char *ota_pw)
	{
		// Setup OTA and wait
		#ifdef ESP8266
		LOGF("Reset reason = %s - %s\n", ESP.getResetReason().c_str());
		#endif
		OTA::setup(ota_pw);
		LOGN("Booted. Waiting for possible OTAs");
		OTA::wait_for_otas();
		LOGN("Stopped waiting");
	}

	void connect(const char *name, const char *ota_pw, const char *telnet_ip, int telnet_port)
	{
		connect_pre();

		// Setup telnet
		Net::setup();
		Telnet::setup(name, telnet_ip, telnet_port);

		connect_post(ota_pw);
	}

	void connect_done()
	{
		// Done
		LOGN("Booted");
	}

	void loop()
	{
		OTA::loop();
		Telnet::loop();
		SemiWebSocket::loop();
	}
}
#pragma once

namespace Main
{
	void add_ap(const char *ssid, const char *pw);

	void connect(const char *name, const char *telnet_pw, const char *telnet_ip, int telnet_port);

	void connect_done();

	void loop();
}
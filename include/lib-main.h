#pragma once

namespace Main
{
	void connect(const char *name, const char *telnet_pw, const char *ssid, const char *pw, const char *telnet_ip, int telnet_port);
	void connect(const char *name, const char *telnet_pw, const char *ssid, const char *pw);

	void connect_done();
}
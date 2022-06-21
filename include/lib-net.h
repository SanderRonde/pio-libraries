#pragma once

#include <string.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#endif
#include <WiFiClient.h>

using namespace std;

namespace Net
{
	String req(const char *host, int port, const char *path, String body);
	String req(const char *host, int port, const char *path);
	String req_secure(const char *host, int port, const char *path, String body);
	String req_secure(const char *host, int port, const char *path);

	String req_auth(const char* secret_key, const char *host, int port, const char *path);

	void setup(const char *ssid, const char *pw);

	void await_wifi();
}
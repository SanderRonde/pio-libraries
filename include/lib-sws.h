#pragma once

#include <string.h>
#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#endif
#include <WiFiClient.h>

class SemiWebSocket
{
private:
	String _ws_id;
	bool _connected;
	int _refresh_interval;
	int _ping_interval;
	const char *_host;
	const char *_secret_key;
	;
	unsigned long _last_refresh;
	unsigned long _last_ping;

public:
	static std::vector<SemiWebSocket *> instances;

	char *get_type(char *payload);

	char *get_data(char *payload);

	void create_server();

	void close();

	void ping();

	void connect();

	void start_server();

	SemiWebSocket(const char *host, const char *secret_key, void (*handler)(String event, String type, String data),
								int refresh_interval, int ping_interval);

	void send_message(String type, String data);

	void loop_self();

	static void loop();
};
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

#include <lib-telnet.h>
#include <lib-net.h>
#include <lib-sws.h>

ESP8266WebServer server(80);

void (*_handler)(String event, String type, String data);

void handle_request()
{
	String arg = server.arg(0);
	LOGF("Got a request with arg \"%s\"\n", arg.c_str());
	int spaces = 0;

	String type = "";
	String data = "";
	for (unsigned int i = 0; i < arg.length(); i++)
	{
		if (arg.c_str()[i] == ' ')
		{
			spaces++;
			continue;
		}
		if (spaces == 0)
		{
			type += String(arg.c_str()[i]);
		}
		else if (spaces == 1)
		{
			data += String(arg.c_str()[i]);
		}
	}

	_handler("message", type, data);
	server.send(200, "text/plain", "OK");
}

char *SemiWebSocket::get_type(char *payload)
{
	unsigned int i;
	char *type = (char *)malloc(sizeof(char) * 500);
	for (i = 0; i < strlen(payload); i++)
	{
		if (payload[i] == ' ')
		{
			break;
		}
		type[i] = payload[i];
	}
	type[i] = '\0';
	return type;
}

char *SemiWebSocket::get_data(char *payload)
{
	unsigned int i;
	int j = 0;
	bool found_space = false;
	char *data = (char *)malloc(sizeof(char) * 500);
	for (i = 0; i < strlen(payload); i++)
	{
		if (found_space)
		{
			data[j] = payload[i];
			j++;
		}
		if (payload[i] == ' ')
		{
			found_space = true;
		}
	}
	data[j] = '\0';
	return data;
}

void SemiWebSocket::create_server()
{
	Net::await_wifi();
	server.on("/ws", handle_request);
	server.begin();
}

void SemiWebSocket::close()
{
	_handler("closed", "", "");
	_connected = false;
}

void SemiWebSocket::ping()
{
	send_message("ping", "");
	_last_ping = millis();
}

void SemiWebSocket::connect()
{
	// wait for WiFi connection
	Net::await_wifi();

	HTTPClient http;
	if (!Net::client.connect(_host, 80))
	{
		Serial.println("connection failed");
		return;
	}
	http.setTimeout(2000);
	String url = "http://" + String(_host) + String("/keyval/websocket");
	http.begin(Net::client, url.c_str());
	http.addHeader("Content-Type", "application/json");

	String data = String("{\"auth\": \"");

	data += _secret_key;
	data += "\"";
	data += ", \"ip\": \"";
	data += Net::ipToString(WiFi.localIP());
	data += "\"}";

	int httpCode = http.POST(data);
	if (httpCode > 0)
	{
		if (httpCode == 200)
		{
			_ws_id = http.getString();
			_connected = true;
			LOGF("Got id %s\n", _ws_id.c_str());
			_handler("connected", "", "");
		}
		else
		{
			LOGF("Got err code %d and msg %s\n", httpCode, http.getString().c_str());
			close();
		}
	}
	else
	{
		LOGF("Connect got sending error %s\n", http.errorToString(httpCode).c_str());
		close();
	}
	_last_refresh = millis();
	_last_ping = millis();
	http.end();
}

void SemiWebSocket::start_server()
{
	Net::await_wifi();
	create_server();
}

SemiWebSocket::SemiWebSocket(const char *host, const char *secret_key, void (*handler)(String event, String type, String data),
														 int refresh_interval, int ping_interval)
{
	_handler = handler;
	_refresh_interval = refresh_interval;
	_ping_interval = ping_interval;
	_host = host;
	_secret_key = secret_key;

	instances.push_back(this);
}

void SemiWebSocket::send_message(String type, String data)
{
	// wait for WiFi connection
	Net::await_wifi();

	HTTPClient http;
	if (!Net::client.connect(_host, 80))
	{
		Serial.println("connection failed");
		return;
	}
	http.setTimeout(2000);
	String url = "http://" + String(_host) + String("/ws");
	http.begin(Net::client, url.c_str());
	http.addHeader("Content-Type", "text/plain");

	String body = _ws_id + " " + type + " " + data;
	int httpCode = http.POST(body);
	if (httpCode > 0)
	{
		if (httpCode != 200)
		{
			LOGF("Got err code %d and msg %s\n", httpCode, http.getString().c_str());
			close();
		}
	}
	else
	{
		LOGF("SendMessage got sending error %s\n", http.errorToString(httpCode).c_str());
		close();
	}
	http.end();
}

void SemiWebSocket::loop_self()
{
	if (!_connected && millis() > _last_refresh + _refresh_interval)
	{
		connect();
	}
	if (_connected && millis() > _last_ping + _ping_interval)
	{
		ping();
	}
	server.handleClient();
}

void SemiWebSocket::loop()
{
	for (size_t i = 0; i < instances.size(); i++)
	{
		instances[i]->loop_self();
	}
}

std::vector<SemiWebSocket *> SemiWebSocket::instances = std::vector<SemiWebSocket *>();
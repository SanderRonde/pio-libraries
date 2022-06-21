#include <string.h>

#include <WiFiClient.h>

#include <lib-net.h>

using namespace std;

String ipToString(IPAddress ip){
	String s = "";
	for (int i = 0; i < 4; i++)
		s += i  ? "." + String(ip[i]) : String(ip[i]);
	return s;
}

#ifdef ESP8266
ESP8266WiFiMulti wifi;
#endif
#ifdef ESP32
WiFiMulti wifi;
#endif

namespace Net {
	WiFiClient client;
	String req(const char* host, int port, const char* path, String body) {
		// wait for WiFi connection
		await_wifi();

		String result = "?";

		HTTPClient http;
		if (!client.connect(host, port))
		{
			Serial.println("connection failed");
			return "";
		}

		http.setTimeout(2000);
		String url = "http://" + String(host) + String(path);
		http.begin(client, url.c_str());
		http.addHeader("Content-Type", "text/plain");

		int httpCode = http.POST(body);
		if(httpCode > 0) {
			if(httpCode != 200) {
				Serial.printf("Got err code %d and msg %s\n", httpCode, http.getString().c_str());
			} else {
				result = http.getString();
			}
		} else {
			Serial.printf("Got sending error %s\n", http.errorToString(httpCode).c_str());
		}
		http.end();

		return result;
	}

	String req(const char* host, int port, const char* path) {
		return req(host, port, path, "");
	}

	String req_auth(const char* secret_key, const char* host, int port, const char* path) {
		// wait for WiFi connection
		await_wifi();

		String result = "?";

		HTTPClient http;
		if (!client.connect(host, port))
		{
			Serial.println("connection failed");
			return "";
		}

		http.setTimeout(2000);
		String url = "http://" + String(host) + String(path);
		http.begin(client, url.c_str());
		http.addHeader("Content-Type", "application/json");

		String data = String("{\"auth\": \"");
		
		data += secret_key;
		data += "\"";
		data += ", \"ip\": \"";
		data += ipToString(WiFi.localIP());
		data += "\"}";

		int httpCode = http.POST(data);
		if(httpCode > 0) {
			if(httpCode != 200) {
				Serial.printf("Got err code %d and msg %s\n", httpCode, http.getString().c_str());
			} else {
				result = http.getString();
			}
		} else {
			Serial.printf("Got sending error %s\n", http.errorToString(httpCode).c_str());
		}
		http.end();

		return result;
	}

	bool _setup = false;
	void setup(const char * ssid, const char * pw) {
		if (_setup) return;

		for (uint8_t t = 4; t > 0; t--) {
			Serial.printf("[SETUP] WAIT %d...\n", t);
			Serial.flush();
			delay(1000);
		}

		WiFi.mode(WIFI_STA);
		wifi.addAP(ssid, pw);

		_setup = true;
	}

	void await_wifi() {
		while (wifi.run() != WL_CONNECTED) {
			delay(1000);
		}
	}
}
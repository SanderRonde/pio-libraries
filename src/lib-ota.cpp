#include <ArduinoOTA.h>
#include <lib-ota.h>

#define OTA_TIME_S 10

namespace OTA
{
	void setup(const char *OTA_PW)
	{
		ArduinoOTA.setPort(8266);
		ArduinoOTA.setPassword(OTA_PW);
		ArduinoOTA.onStart([]()
											 {
												 String type;
												 if (ArduinoOTA.getCommand() == U_FLASH)
												 {
													 type = "sketch";
												 }
												 else
												 { // U_FS
													 type = "filesystem";
												 }

												 // NOTE: if updating FS this would be the place to unmount FS using FS.end()
												 Serial.println("Start updating " + type);
											 });
		ArduinoOTA.onEnd([]()
										 { Serial.println("\nEnd"); });
		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
													{ Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
		ArduinoOTA.onError([](ota_error_t error)
											 {
												 Serial.printf("Error[%u]: ", error);
												 if (error == OTA_AUTH_ERROR)
												 {
													 Serial.println("Auth Failed");
												 }
												 else if (error == OTA_BEGIN_ERROR)
												 {
													 Serial.println("Begin Failed");
												 }
												 else if (error == OTA_CONNECT_ERROR)
												 {
													 Serial.println("Connect Failed");
												 }
												 else if (error == OTA_RECEIVE_ERROR)
												 {
													 Serial.println("Receive Failed");
												 }
												 else if (error == OTA_END_ERROR)
												 {
													 Serial.println("End Failed");
												 }
											 });
		ArduinoOTA.begin();
		Serial.println("OTA listening");
	}

	void wait_for_otas()
	{
		unsigned long start_time = millis();
		while (millis() < start_time + (OTA_TIME_S * 1000))
		{
			ArduinoOTA.handle();
			yield();
		}
	}

	void loop()
	{
		ArduinoOTA.handle();
	}
}
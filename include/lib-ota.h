#pragma once

namespace OTA {
	void setup(const char *OTA_PW);

	void wait_for_otas();

	void loop();
}
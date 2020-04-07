#pragma once
#include "ILogger.h"
#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"

class DisplayLogger : public ILogger {
public:
	void LogError(std::string message) override;
	void LogDebug(std::string message) override;
	void LogCount(std::string message) override;
	void LogMessage(std::string message) override;
	static void Initialize();
private:
	ArduiPi_OLED display;
};

inline void DisplayLogger::LogError(std::string message) {
	// TODO: implement
}

inline void DisplayLogger::LogDebug(std::string message) {
	// TODO: implement
}

inline void DisplayLogger::LogCount(std::string message) {
	// TODO: implement
}

inline void DisplayLogger::LogMessage(std::string message) {
	// TODO: implement
}

inline void DisplayLogger::Initialize() {
	display.init(OLED_I2C_RESET, 3);
	display.begin();
	display.clearDisplay();
	display.display();
	display.setTextSize(1);
}
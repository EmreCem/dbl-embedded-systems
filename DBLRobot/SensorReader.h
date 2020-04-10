#pragma once
#include <vector>

class SensorReader {
public:
	static void Setup(std::vector<int> pins);
	static int ReadTapeRGBSensor();
	static int ReadFactoryRGBSensor();
	static bool ReadFactoryPresenceSensor();
	static bool ReadTapePresenceSensor();
};

inline void SensorReader::Setup(std::vector<int> pins) {
	for (int pin : pins) {
		pinMode(pin, OUTPUT);
	}
}

inline int SensorReader::ReadTapeRGBSensor() {
	//return digitalRead();
	return rand() % 2;
}

inline int SensorReader::ReadFactoryRGBSensor() {
	//return digitalRead();
}

inline bool SensorReader::ReadFactoryPresenceSensor() {
	//return digitalRead();
}

inline bool SensorReader::ReadTapePresenceSensor() {
	//return digitalRead();
}
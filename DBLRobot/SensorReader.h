#pragma once
#include <vector>

class SensorReader {
public:
	static void Setup(std::vector<int> pins);
	static int ReadRGBSensor();
	static bool ReadFactoryPresenceSensor();
	static bool ReadBeltPresenceSensor();
};

inline void SensorReader::Setup(std::vector<int> pins) {
	for (int pin : pins) {
		pinMode(pin, OUTPUT);
	}
}

inline int SensorReader::ReadRGBSensor() {
	return digitalRead();
}

inline bool SensorReader::ReadFactoryPresenceSensor() {
	return digitalRead();
}

inline bool SensorReader::ReadBeltPresenceSensor() {
	return digitalRead();
}

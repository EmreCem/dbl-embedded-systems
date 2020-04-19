#pragma once
#include <vector>
#include "wiringPi.h"
#include "Globals.h"
#include "serial/serial.h"
#include "ILogger.h"
#include <exception>

enum Color {
	WHITE,
	BLACK,
	PINK,
	BELT,
	UNKNOWN
};

class SensorReader {
	static ILogger* _logger;

	static std::vector<int> CalibratedPink;
	static std::vector<int> CalibratedBeltWhite;
	static std::vector<int> CalibratedBeltBlack;
	static std::vector<int> CalibratedFactoryWhite;
	static std::vector<int> CalibratedFactoryBlack;
	static std::vector<int> CalibratedBelt;
	
	static serial::Serial myserial;
	static Color ConvertValuesToColor(int r, int g, int b, bool isBelt = true);
	static int ConvertStringToNumber(std::string value);
	static std::vector<int> ReadRGBSensorValues(char sensor);
public:
	static void Setup(ILogger* logger);
	static Color ReadBeltRGBSensor();
	static Color ReadFactoryRGBSensor();
	static bool ReadFactoryPresenceSensor();
	static bool ReadTapePresenceSensor();
	static void CalibratePink();
	static void CalibrateBeltWhite();
	static void CalibrateBeltBlack();
	static void CalibrateFactoryWhite();
	static void CalibrateFactoryBlack();
	static void CalibrateBelt();
};

serial::Serial SensorReader::myserial("/dev/ttyACM0");
ILogger* SensorReader::_logger = 0;
std::vector<int> SensorReader::CalibratedPink;
std::vector<int> SensorReader::CalibratedBeltWhite;
std::vector<int> SensorReader::CalibratedBeltBlack;
std::vector<int> SensorReader::CalibratedFactoryWhite;
std::vector<int> SensorReader::CalibratedFactoryBlack;
std::vector<int> SensorReader::CalibratedBelt;

inline Color SensorReader::ConvertValuesToColor(int r, int g, int b, bool isBelt) {
	if (!isBelt) {
		if (std::abs(CalibratedFactoryWhite[0] - r) < 66 &&
			std::abs(CalibratedFactoryWhite[1] - g) < 66 &&
			std::abs(CalibratedFactoryWhite[2] - b) < 66) {
			return WHITE;
		}

		if (std::abs(CalibratedFactoryBlack[0] - r) < 66 &&
			std::abs(CalibratedFactoryBlack[1] - g) < 66 &&
			std::abs(CalibratedFactoryBlack[2] - b) < 66) {
			return BLACK;
		}

		return UNKNOWN;
	}

	
	if (std::abs(CalibratedPink[0] - r) < 66 &&
		std::abs(CalibratedPink[1] - g) < 66 &&
		std::abs(CalibratedPink[2] - b) < 66) {
		return PINK;
	}

	if (std::abs(CalibratedBeltWhite[0] - r) < 66 &&
		std::abs(CalibratedBeltWhite[1] - g) < 66 &&
		std::abs(CalibratedBeltWhite[2] - b) < 66) {
		return WHITE;
	}
	
	if (std::abs(CalibratedBeltBlack[0] - r) < 66 &&
		std::abs(CalibratedBeltBlack[1] - g) < 66 &&
		std::abs(CalibratedBeltBlack[2] - b) < 66) {
		return BLACK;
	}
	
	if (std::abs(CalibratedBelt[0] - r) < 66 &&
		std::abs(CalibratedBelt[1] - g) < 66 &&
		std::abs(CalibratedBelt[2] - b) < 66) {
		return BELT;
	}

	return UNKNOWN;
}

inline int SensorReader::ConvertStringToNumber(std::string value) {
	//_logger->LogDebug("Converting " + value + " to int.");
	int result = std::stoi(value.substr(2, value.size() - 2));
	return result;
}

inline std::vector<int> SensorReader::ReadRGBSensorValues(char sensor) {
	std::vector<int> values;

	try {
		myserial.flushInput();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		std::string buf;
		myserial.readline(buf);
		buf.clear();
		myserial.readline(buf);

		while (buf[0] != sensor || buf.size() < 15) {
			buf.clear();
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			myserial.readline(buf);
		}

		_logger->LogDebug("Values from sensor: " + buf.substr(0, buf.size() - 2));

		int firstSpace = buf.find_first_of(' ');
		int secondSpace = buf.find_last_of(' ');
		int r = ConvertStringToNumber(buf.substr(0, firstSpace));
		int g = ConvertStringToNumber(buf.substr(firstSpace + 1, secondSpace - firstSpace - 1));
		int b = ConvertStringToNumber(buf.substr(secondSpace + 1, buf.size() - 2 - secondSpace));

		values.push_back(r);
		values.push_back(g);
		values.push_back(b);
		return values;
	} catch (std::exception Exception) {
		_logger->LogError("Exception caught while reading RGB sensor, retrying.");
		return ReadRGBSensorValues(sensor);
	}
}

inline void SensorReader::Setup(ILogger* logger) {
	_logger = logger;
	pinMode(Globals::BeltPresenceSensorPin, INPUT);
	pinMode(Globals::FactoryPresenceSensorPin, INPUT);
}

inline Color SensorReader::ReadBeltRGBSensor() {
	//_logger->LogDebug("Reading belt RGB sensor.");

	auto values = ReadRGBSensorValues('T');

	//_logger->LogDebug("Converted values: " + std::to_string(values[0]) + " " + std::to_string(values[1]) + " " + std::to_string(values[2]));
	return ConvertValuesToColor(values[0], values[1], values[2]);
}

inline Color SensorReader::ReadFactoryRGBSensor() {
	//_logger->LogDebug("Reading factory RGB sensor.");
	
	auto values = ReadRGBSensorValues('F');

	//_logger->LogDebug("Converted values: " + std::to_string(values[0]) + " " + std::to_string(values[1]) + " " + std::to_string(values[2]));
	return ConvertValuesToColor(values[0], values[1], values[2], false);
}

inline bool SensorReader::ReadFactoryPresenceSensor() {
	return digitalRead(Globals::FactoryPresenceSensorPin);
}

inline bool SensorReader::ReadTapePresenceSensor() {
	return digitalRead(Globals::BeltPresenceSensorPin);
}

inline void SensorReader::CalibratePink() {
	_logger->LogDebug("Callibrating Pink color.");
	auto values = ReadRGBSensorValues('T');
	CalibratedPink = values;
	_logger->LogDebug("Successfully calibrated pink.");
}

inline void SensorReader::CalibrateBeltWhite() {
	_logger->LogDebug("Callibrating White color.");
	auto values = ReadRGBSensorValues('T');
	CalibratedBeltWhite = values;
	_logger->LogDebug("Successfully calibrated belt white.");
}

inline void SensorReader::CalibrateBeltBlack() {
	_logger->LogDebug("Callibrating Black color.");
	auto values = ReadRGBSensorValues('T');
	CalibratedBeltBlack = values;
	_logger->LogDebug("Successfully calibrated belt black.");
}

inline void SensorReader::CalibrateFactoryWhite() {
	_logger->LogDebug("Callibrating White color.");
	auto values = ReadRGBSensorValues('F');
	CalibratedFactoryWhite = values;
	_logger->LogDebug("Successfully calibrated factory white.");
}

inline void SensorReader::CalibrateFactoryBlack() {
	_logger->LogDebug("Callibrating Black color.");
	auto values = ReadRGBSensorValues('F');
	CalibratedFactoryBlack = values;
	_logger->LogDebug("Successfully calibrated factory black.");
}

inline void SensorReader::CalibrateBelt() {
	_logger->LogDebug("Callibrating Belt color.");
	auto values = ReadRGBSensorValues('T');
	CalibratedBelt = values;
	_logger->LogDebug("Successfully calibrated belt.");
}

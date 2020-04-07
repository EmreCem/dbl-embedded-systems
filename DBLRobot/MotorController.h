#pragma once
#include <wiringPi.h>
#include <vector>

class MotorController {
public:
	static void Setup(std::vector<int> pins);
	static void MoveMotorClockwise(int leftPin, int rightPin);
	static void MoveMotorCounterClockwise(int leftPin, int rightPin);
	static void StopMotor(int leftPin, int rightPin);
	static void StopAllMotors();
};


inline void MotorController::Setup(std::vector<int> pins) {
	for (int pin : pins) {
		pinMode(pin, OUTPUT);
	}
}

inline void MotorController::MoveMotorClockwise(int leftPin, int rightPin) {
	digitalWrite(leftPin, 1);
	digitalWrite(rightPin, 0);
}

inline void MotorController::MoveMotorCounterClockwise(int leftPin, int rightPin) {
	digitalWrite(leftPin, 0);
	digitalWrite(rightPin, 1);
}

inline void MotorController::StopMotor(int leftPin, int rightPin) {
	digitalWrite(leftPin, 0);
	digitalWrite(rightPin, 0);
}

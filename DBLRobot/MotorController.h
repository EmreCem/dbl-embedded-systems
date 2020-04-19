#pragma once
#include "wiringPi.h"
#include <vector>

class MotorController {
private:
	int leftPin = 0;
	int rightPin = 0;
public:
	MotorController(int leftPin, int rightPin);
	void Setup();
	void MoveClockwise();
	void MoveCounterClockwise();
	void Stop();
};


inline MotorController::MotorController(int leftPin, int rightPin) {
	this->leftPin = leftPin;
	this->rightPin = rightPin;
}

inline void MotorController::Setup() {
	pinMode(leftPin, OUTPUT);
	pinMode(rightPin, OUTPUT);
}

inline void MotorController::MoveClockwise() {
	digitalWrite(leftPin, 1);
	digitalWrite(rightPin, 0);
}

inline void MotorController::MoveCounterClockwise() {
	digitalWrite(leftPin, 0);
	digitalWrite(rightPin, 1);
}

inline void MotorController::Stop() {
	digitalWrite(leftPin, 0);
	digitalWrite(rightPin, 0);
}

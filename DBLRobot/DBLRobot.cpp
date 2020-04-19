// DBLRobot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <chrono>
#include <thread>
#include <iostream>
#include "ILogger.h"
#include "ConsoleLogger.h"
#include "MotorController.h"
#include "SensorReader.h"
#include "Globals.h"
#include <wiringPi.h>
//#include "Client.h"
#include <string>

ConsoleLogger logger{};
SensorReader sensor{};
MotorController beltMotor(Globals::BeltMotorLeftPin, Globals::BeltMotorRightPin);
MotorController pusherMotor(Globals::BeltPusherLeftPin, Globals::BeltPusherRightPin);
MotorController factoryBeltMotor(Globals::FactoryBeltLeftPin, Globals::FactoryBeltRightPin);
MotorController factoryPusherMotor(Globals::FactoryPusherLeftPin, Globals::FactoryPusherRightPin);

void LogicLoop();
std::string DiskBinary(int Num) // Returns string with disk colors needed to make the number
{
	std::string DiskString;

	while (Num != 0) {
		if (Num % 2 == 0) {
			DiskString = 'b' + DiskString;
		} else {
			DiskString = 'w' + DiskString;
		}
		Num /= 2;
	}

	while (DiskString.size() < 3) {
		DiskString.insert(0, "e");
	}
	return DiskString;
}

//void HandleEmergency(MotorController* motor = nullptr, bool wasClockwise = true) {
//	bool isEmergency = false;
//	while (client.isEmergency) {
//		if (motor != nullptr) {
//			motor->Stop();
//		}
//		isEmergency = true;
//		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//	}
//
//	if (isEmergency) {
//		logger.LogDebug("Resuming operation.");
//		if (wasClockwise && motor != nullptr) {
//			motor->MoveClockwise();
//		} else if (motor != nullptr) {
//			motor->MoveCounterClockwise();
//		}
//	}
//}

void HandleFault() {
	logger.LogError("Error detected. Please fix within 25 seconds.");
	beltMotor.Stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(25000));
	logger.LogDebug("25 seconds elapsed, continuing operation.");
}

void DetectFaultWhileMoving(int pos, std::string currentDisks, bool goingRight) {
	bool faultFound = false;
	bool isDiskDetected = false;
	bool wasDiskDetected = false;
	bool shouldDiskBeDetected = true;
	Color diskColor = UNKNOWN;
	int diskIndex = (currentDisks.size() - 1) - pos;

	if (!goingRight) {
		diskIndex++;
	}

	if (currentDisks[diskIndex] == 'w') {
		logger.LogDebug("Disk on position " + std::to_string(diskIndex) + " should be white.");
		diskColor = WHITE;
	} else if (currentDisks[diskIndex] == 'b') {
		logger.LogDebug("Disk on position " + std::to_string(diskIndex) + " should be black.");
		diskColor = BLACK;
	} else {
		logger.LogDebug("There should be no disk in position " + std::to_string(diskIndex));
		shouldDiskBeDetected = false;
	}

	while (true) {
		//HandleEmergency(&beltMotor, goingRight);
		
		// Check if the tape is on the next border
		auto rgbValue = sensor.ReadBeltRGBSensor();
		if (rgbValue == PINK) break;

		// fault detection
		isDiskDetected = sensor.ReadTapePresenceSensor();
		if (isDiskDetected) {
			wasDiskDetected = true;
			if (!shouldDiskBeDetected) {
				faultFound = true;
				logger.LogError(
					"Fault found: Disk that shouldn't be on the belt is there in position: " + std::
					to_string(diskIndex));
				HandleFault();
				if (goingRight) beltMotor.MoveClockwise();
				else beltMotor.MoveCounterClockwise();
				faultFound = false;
			}

			if (rgbValue == WHITE && diskColor == BLACK) {
				faultFound = true;
				logger.LogError(
					"Fault found: White disk found instead of black in position: " + std::to_string(diskIndex));
				HandleFault();
				if (goingRight) beltMotor.MoveClockwise();
				else beltMotor.MoveCounterClockwise();
				faultFound = false;
			}

			if (rgbValue == BLACK && diskColor == WHITE) {
				faultFound = true;
				logger.LogError(
					"Fault found: Black disk found instead of white in position: " + std::to_string(diskIndex));
				HandleFault();
				if (goingRight) beltMotor.MoveClockwise();
				else beltMotor.MoveCounterClockwise();
				faultFound = false;
			}
		}
	}

	if (!wasDiskDetected && shouldDiskBeDetected) {
		faultFound = true;
		logger.LogError("Fault found: No disk found at position: " + std::to_string(diskIndex));
	}

	if (faultFound) {
		HandleFault();
		if (goingRight) beltMotor.MoveClockwise();
		else beltMotor.MoveCounterClockwise();
	}
}

int MoveBeltRight(int CurrRampPos, std::string currentDisks) {
	// Moves the tape right
	// Always end on paper, so first we are on pink
	logger.LogDebug("On pink");
	beltMotor.MoveClockwise();
	logger.LogDebug("Started moving right");
	//HandleEmergency(&beltMotor, true);
	std::this_thread::sleep_for(std::chrono::milliseconds(8000));
	//HandleEmergency(&beltMotor, true);
	DetectFaultWhileMoving(CurrRampPos, currentDisks, true);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	beltMotor.Stop();
	logger.LogDebug("Shifted Right!");
	return CurrRampPos + 1;
}

int MoveBeltLeft(int CurrRampPos, std::string currentDisks) {
	// Moves the tape left
	// Always end on pink piece of paper, so we're on it rn
	logger.LogDebug("On pink");
	beltMotor.MoveCounterClockwise();
	logger.LogDebug("Started moving left");
	std::this_thread::sleep_for(std::chrono::milliseconds(8000));
	logger.LogDebug("Between pink");

	DetectFaultWhileMoving(CurrRampPos, currentDisks, false);

	logger.LogDebug("On pink");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	beltMotor.Stop();
	logger.LogDebug("Shifted left!");
	return CurrRampPos - 1;
}

void PushOffBelt() {
	logger.LogDebug("Pushing disk off belt.");
	pusherMotor.MoveClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	pusherMotor.Stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	pusherMotor.MoveCounterClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	pusherMotor.Stop();
	logger.LogDebug("Disk removed!");
}

void PushOffFactory() {
	//HandleEmergency();
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	//HandleEmergency();
	// First move to block the belt
	factoryPusherMotor.MoveCounterClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	factoryPusherMotor.Stop();
	
	//HandleEmergency();	
	// Wait for disk to get into the slot
	std::this_thread::sleep_for(std::chrono::milliseconds(13000));
	//HandleEmergency();
	
	logger.LogDebug("Start swinging");
	factoryPusherMotor.MoveClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	factoryPusherMotor.Stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	logger.LogDebug("Disk pushed!");
}

void FindDisk(char Disk) {
	bool found = false;
	logger.LogDebug("Getting a disk off the factory floor.");
	
	while (!found) {
		//HandleEmergency();
		while (!sensor.ReadFactoryPresenceSensor()) {
			/*if (client.shouldStop) {
				logger.LogDebug("Stopping fetching a disk.");
				break;
			*///}

			//HandleEmergency();
			
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		if (Disk == 'b') {
			if (sensor.ReadFactoryRGBSensor() == BLACK) {
				logger.LogDebug("Black disk found, taking off.");
				found = true;		
				PushOffFactory();
				//if (!solo) {
				//	//client.diskTaken();
				//}
			}
		}

		if (Disk == 'w') {
			if (sensor.ReadFactoryRGBSensor() == WHITE) {
				logger.LogDebug("White disk found");
				found = true;
				PushOffFactory();
				//if (!solo) {
				//	//client.diskTaken();
				//}
			}
		}
	}
}

void Calibrate() {
	char robotId;
	logger.LogDebug(
		"Calibration started.\n(0) Stop belt.\n(1) Move belt to the right\n(2) Move belt to the left\n(3) Calibrate belt pink\n(4) Calibrate belt white\n(5) Calibrate belt black\n(6) Calibrate belt\n(7) Calibrate factory white\n(8) Calibrate factory black.\n(9) Finish calibration and start (solo).\n(10) Connect MQTT.\n(11) Finish calibration.\n(12) Reconnect");
	bool calibrating = true;

	pusherMotor.MoveCounterClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	pusherMotor.Stop();

	factoryPusherMotor.MoveClockwise();
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	factoryPusherMotor.Stop();

	while (calibrating) {
		int i = -1;
		std::cin >> i;

		switch (i) {
		case 0:
			beltMotor.Stop();
			factoryBeltMotor.Stop();
			break;
		case 1:
			beltMotor.MoveClockwise();
			break;
		case 2:
			beltMotor.MoveCounterClockwise();
			break;
		case 3:
			sensor.CalibratePink();
			break;
		case 4:
			sensor.CalibrateBeltWhite();
			break;
		case 5:
			sensor.CalibrateBeltBlack();
			break;
		case 6:
			sensor.CalibrateBelt();
			break;
		case 7:
			sensor.CalibrateFactoryWhite();
			break;
		case 8:
			sensor.CalibrateFactoryBlack();
			break;
		case 9:
			calibrating = false;
			LogicLoop();
			break;
		case 10:
			//solo = false;
			//std::cout << "Please enter the robot's ID: ";
			//std::cin >> robotId;
			//clientThread = std::thread(&Client::main_loop, &client, robotId, &factoryBeltMotor);
			break;
		case 12:
			//client.requestDisksTaken();
			break;
		default:
			break;
		}
	}

	logger.LogDebug("Calibration finished.");
}

int LatestDiskDifference(std::string& neededDisks, std::string& currentDisks) {
	logger.LogDebug("currentDisks = " + currentDisks + ", numDisks = " + neededDisks);

	while (neededDisks.size() > currentDisks.size()) {
		currentDisks.insert(0, "e");
	}
	logger.LogDebug("currentDisks = " + currentDisks + ", numDisks = " + neededDisks);

	for (int i = 0; i < neededDisks.size(); i++) {
		if (neededDisks[i] != currentDisks[i]) {
			logger.LogDebug("Wrong at index " + std::to_string(i));
			return neededDisks.size() - 1 - i;
		}
	}
}

// ramp pos is 0 on the left, 3 on the far right

void MoveBeltToDesiredPos(int* rampPos, int neededPos, std::string currentDisks) {
	while (*rampPos != neededPos) {
		// Shift right position under the ramp
		if (neededPos > *rampPos) {
			*rampPos = MoveBeltRight(*rampPos, currentDisks);
		} else {
			*rampPos = MoveBeltLeft(*rampPos, currentDisks);
		}
		logger.LogDebug("Ramp pos: " + std::to_string(*rampPos));
	}
}

void PushOffAll(std::string currentDisks, int rampPos) {
	int neededPos;
	for (int i = currentDisks.size() - 1; i >= 0; --i) {
		if (currentDisks[i] != 'e') {
			logger.LogDebug("Disk on position " + std::to_string(currentDisks.size() - 1 - i) + " is coming off.");
			neededPos = currentDisks.size() - i;
			logger.LogDebug("Needed pos to remove the disk: " + std::to_string(neededPos));
			MoveBeltToDesiredPos(&rampPos, neededPos, currentDisks);
			logger.LogDebug("Pushing off useless disk.");
			PushOffBelt();
			currentDisks[i] = 'e';
		}
	}
}

void LogicLoop() {
	int num = 1;
	std::string numAsDisks;
	std::string currentDisks = "eee";
	char neededDisk;
	int neededPos = 0; // position at which the disk needs to be placed
	int rampPos = 0; // default position after calibration
	int numLength; // how many disks needed to form number
	bool initial = true;

	// taken care of by mqtt
	factoryBeltMotor.MoveClockwise();
	
	while (num <= 7) {
		numAsDisks = DiskBinary(num);

		logger.LogDebug("Number to form: " + std::to_string(num));

		if (!initial) {
			logger.LogDebug("Checking if any disks are to be removed.");
			for (int i = currentDisks.size() - 1; i >= 0; --i) {
				if (currentDisks[i] != numAsDisks[i] && currentDisks[i] != 'e') {
					logger.LogDebug("Disk on position " + std::to_string(currentDisks.size() - 1 - i) + " is wrong.");
					neededPos = currentDisks.size() - i;
					logger.LogDebug("Needed pos to remove the disk: " + std::to_string(neededPos));
					MoveBeltToDesiredPos(&rampPos, neededPos, currentDisks);
					logger.LogDebug("Pushing off useless disk.");
					PushOffBelt();
					currentDisks[i] = 'e';
				}
			}
		}

		/*if (client.shouldStop) {
			PushOffAll(currentDisks, rampPos);
			break;
		}*/

		//HandleEmergency();
		
		while (numAsDisks != currentDisks) {
			// Keep collecting disks until all needed disks are aquired
			logger.LogDebug("Number as disks: " + numAsDisks);
			neededPos = LatestDiskDifference(numAsDisks, currentDisks);
			logger.LogDebug("Needed position to put new disk: " + std::to_string(neededPos));
			logger.LogDebug("Current ramp pos: " + std::to_string(rampPos));

			/*if (client.shouldStop) {
				PushOffAll(currentDisks, rampPos);
				num = 10;
				break;
			}*/

			//HandleEmergency();
			
			MoveBeltToDesiredPos(&rampPos, neededPos, currentDisks);

			/*if (client.shouldStop) {
				PushOffAll(currentDisks, rampPos);
				num = 10;
				break;
			}*/

			//HandleEmergency();
			
			neededDisk = numAsDisks[numAsDisks.size() - 1 - neededPos]; // Disk needed to form binary number
			logger.LogDebug("Needed Disk: " + neededDisk);

			// Get needed disk from the conveyor belt
			/*if (!solo) {
				while (!client.allowedToTakeDisk()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(400));
				}
			}*/
			FindDisk(neededDisk);
			currentDisks[currentDisks.size() - 1 - neededPos] = neededDisk;
			logger.LogDebug("Num as disks: " + numAsDisks + ", currentDisks = " + currentDisks);
		}

		logger.LogDebug("Finalized showing number, incrementing.");
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		++num;
		initial = false;

		if (num == 8) {
			num = 1;
		}
	}
}

int main() {
	wiringPiSetupGpio();
	sensor.Setup(&logger);
	beltMotor.Setup();
	pusherMotor.Setup();
	factoryBeltMotor.Setup();
	factoryPusherMotor.Setup();

	Calibrate();

	while (true) {

	}

	return 1;
}

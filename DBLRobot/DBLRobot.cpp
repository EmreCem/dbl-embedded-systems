// DBLRobot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <chrono>
#include <thread>
#include <iostream>
#include "ILogger.h"
#include "ConsoleLogger.h"
#include "MotorController.h"
#include "SensorReader.h"

ConsoleLogger logger{};
SensorReader sensor{};
MotorController motor{};

std::string DiskBinary(int Num) // Returns string with disk colors needed to make the number
{
    std::string DiskString;

    if (Num == 0)
    {
        return "b";
    }

    while (Num != 0) {
        if (Num % 2 == 0) {
            DiskString = 'b' + DiskString;
        }
        else {
            DiskString = 'w' + DiskString;
        }
        Num /= 2; 
    }

    return DiskString;
}

int MoveRight(int CurrRampPos) { // Moves the tape right
    // Always end on border, so first we are on border
    logger.LogDebug("On border");
    motor.MoveMotorClockwise(1, 2); //Fill in right pins
    logger.LogDebug("Started moving Right");
    while (sensor.ReadTapeRGBSensor() == 1) { // Check if the tape is on a border
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    logger.LogDebug("On slot");
    while (sensor.ReadTapeRGBSensor() == 0) // Check if the tape is on a slot
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    logger.LogDebug("On border");
    motor.StopMotor(1, 2);
    logger.LogDebug("Stopped moving right");
    logger.LogDebug("Shifted Right!");
    return CurrRampPos - 1;
}

int MoveLeft(int CurrRampPos) { // Moves the tape left
    // Always end on border, so first we are on border
    logger.LogDebug("On border");
    motor.MoveMotorCounterClockwise(1, 2); //Fill in right pins
    logger.LogDebug("Started moving Left");
    while (sensor.ReadTapeRGBSensor() == 1) { // Check if the tape is on a border
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    logger.LogDebug("On slot");
    while (sensor.ReadTapeRGBSensor() == 0) // Check if the tape is on a slot
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    logger.LogDebug("On border");
    motor.StopMotor(1, 2);
    logger.LogDebug("Stopped moving left");
    logger.LogDebug("Shifted left!");
    return CurrRampPos + 1;
}

void PushTape() {
    logger.LogDebug("Disk removed!");
}

void PushBelt() {
    logger.LogDebug("Disk pushed!");
}

bool SenseBelt() { // Check if there is a disk on the belt
    //sense for disk
    // if found
    logger.LogDebug("Disk sensed on belt!\n\"t");
    return true;
    // else return false
}

bool CheckWhite() {
    // Check if color is white
    // if white
    logger.LogDebug("White disk found!");
    return true;
    // else return false
}

bool CheckBlack() {
    // Check if color is black
    // if black
    logger.LogDebug("Black disk found!");
    return true;
    // else return false
}

void FindDisk(char Disk) {
    bool found = false;
    if (Disk == 'b') {
        while (!found) {
            while (!SenseBelt) {}
            found = CheckBlack();
        }
        // push if black
        PushBelt();
    }

    if (Disk == 'w') {
        while (!found) {
            while (!SenseBelt) {}
            found = CheckWhite();
        }
        // push if black
        PushBelt();
    }
}

int main() {
    int Num = 0;
    std::string DiskString;
    char CurrDisks[7];
    char NeededDisk;
    int NeededPos; // Position at which the disk needs to be placed
    int RampPos = 4; // The current position under the ramp on the tape
    int SensorPos = 6; // The current position under the rgb sensor on the tape. 
    int length; // number of spaces needed to form number
    while (Num <= 7) {
        DiskString = DiskBinary(Num); // Needed disks
        length = DiskString.length(); // Number of spaces needed on tape
        std::cout << "\n";
        logger.LogDebug("Number to form: " + std::to_string(Num));
        while (!DiskString.empty()) { // Keep collecting disks until all needed disks are aquired
            logger.LogDebug("Needed Disks: " + DiskString);
            NeededPos = 4 + DiskString.length() - length; // Needed pos for current disk
            logger.LogDebug("Needed position: " + std::to_string(NeededPos));
            logger.LogDebug("RampPos: " + std::to_string(RampPos));
            while (RampPos != NeededPos){ // Shift right position under the ramp
                if (NeededPos > RampPos) {
                    // Shift conveyor left
                    RampPos = MoveLeft(RampPos);
                }
                else {
                    // Shift conveyor right
                    RampPos = MoveRight(RampPos);
                }
                logger.LogDebug("RampPos: " + std::to_string(RampPos));
            }

            NeededDisk = DiskString.back(); // Disk needed to form binary number
            logger.LogDebug("Needed Disk: " + std::to_string(NeededDisk));

            // Check for Needed Disk on conveyor belt
            FindDisk(NeededDisk);
            
            // Save value in currDisk at currPos
            
            DiskString.pop_back(); // Remove found disk from DiskString
        }
        // Start to remove disks from conveyor belt
        // Pusher is between the ramp and the sensor
        // Shift right to push the most left disk off
        RampPos = MoveRight(RampPos);
        while (RampPos != 4) {
            // Push disk off tape and move left
            PushTape();
            RampPos = MoveLeft(RampPos);
        }
        Num++;
    }

    
    //wiringPiSetupGpio();
    //ConsoleLogger logger{};
}
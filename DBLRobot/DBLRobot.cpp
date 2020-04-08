// DBLRobot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include "ILogger.h"
#include "ConsoleLogger.h"
//#include "MotorController.h"
//#include "SensorReader.h"

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
    //Sense whether on border
    //On border
        // Move till belt
        // Move till next border
    //Not on border
        // Move till border
        // Move till belt
        // Move till next border
    std::cout << "Shifted right!\n\t";
    return CurrRampPos - 1;
}

int MoveLeft(int CurrRampPos) { // Moves the tape left
    //Sense whether on border
    //On border
        // Move till belt
        // Move till next border
    //Not on border
        // Move till border
        // Move till belt
        // Move till next border
    std::cout << "Shifted left!\n\t";
    return CurrRampPos + 1;
}

void PushTape() {
    std::cout << "Disk removed!\n\t";
}

void PushBelt() {
    std::cout << "Disk pushed!\n\t";
}

bool SenseBelt() { // Check if there is a disk on the belt
    //sense for disk
    // if found
    std::cout << "Disk sensed on belt!\n\"t";
    return true;
    // else return false
}

bool CheckWhite() {
    // Check if color is white
    // if white
    std::cout << "White disk found!\n\t";
    return true;
    // else return false
}

bool CheckBlack() {
    // Check if color is black
    // if black
    std::cout << "Black disk found!\n\t";
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
        std::cout << "\nNumber to form: " << Num << "\n\t";
        while (!DiskString.empty()) { // Keep collecting disks until all needed disks are aquired
            std::cout << "Needed Disks: " << DiskString << "\n\t";
            NeededPos = 4 + DiskString.length() - length; // Needed pos for current disk
            std::cout << "Needed position: " << NeededPos << "\n\t";
            std::cout << "RampPos: " << RampPos << "\n\t";
            while (RampPos != NeededPos){ // Shift right position under the ramp
                if (NeededPos > RampPos) {
                    // Shift conveyor left
                    RampPos = MoveLeft(RampPos);
                }
                else {
                    // Shift conveyor right
                    RampPos = MoveRight(RampPos);
                }
                std::cout << "RampPos: " << RampPos << "\n\t";
            }

            NeededDisk = DiskString.back(); // Disk needed to form binary number
            std::cout << "Needed disk: " << NeededDisk << "\n\t";

            // Check for Needed Disk on conveyor belt
            FindDisk(NeededDisk);
            
            // Save value in currDisk at currPos
            
            DiskString.pop_back(); // Remove found disk from DiskString
        }
        // Start to remove disks from conveyor belt
        // Pusher is between the ramp and the sensor
        // Shift right to push the most left disk off
        RampPos--;
        std::cout << "Shifted right!\n\t";
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
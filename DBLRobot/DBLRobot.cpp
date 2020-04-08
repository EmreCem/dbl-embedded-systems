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
        std::cout << "Number to form: " << Num << "\n\t" << "Needed Disks: " << DiskString << "\n\t";
        while (!DiskString.empty()) { // Keep collecting disks until all needed disks are aquired
            NeededPos = 4 + DiskString.length() - length; // Needed pos for current disk
            std::cout << "Needed position: " << NeededPos << "\n\t";
            std::cout << "RampPos: " << RampPos << '\n\t';
            while (RampPos != NeededPos){ // Shift right position under the ramp
                if (NeededPos > RampPos) {
                    // Shift conveyor left
                    RampPos++;
                }
                else {
                    // Shift conveyor right
                    RampPos--;
                }
                std::cout << "RampPos: " << RampPos << '\n\t';
            }

            NeededDisk = DiskString.back(); // Disk needed to form binary number
            std::cout << "Needed disk: " << NeededDisk << "\n\t";

            // Check for Needed Disk on conveyor belt
            std::cout << "Disk found!\n\t";
            // If found push on tape
            std::cout << "Disk pushed!\n\n";
            
            // Save value in currDisk at currPos
            
            DiskString.pop_back(); // Remove found disk from DiskString
        }
        // Start to remove disks from conveyor belt
        // Pusher is between the ramp and the sensor
        // Shift right to push the most left disk off
        RampPos--;
        while (RampPos != 4) {
            // Push disk off tape and move left
            RampPos++;
        }
        Num++;
    }

    
    //wiringPiSetupGpio();
    //ConsoleLogger logger{};
}
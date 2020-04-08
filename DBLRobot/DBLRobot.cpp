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
    int RampPos = 4; // The current position under the ramp on the tape
    int SensorPos = 6; // The current position under the rgb sensor on the tape. 
    while (Num <= 7) {
        DiskString = DiskBinary(Num);
        std::cout << Num << "\t" << DiskString << "\n";
        while (!DiskString.empty()) {
            NeededDisk = DiskString.back(); // Disk needed to form binary number
            // Check for Needed Disk on conveyor belt
            // If found push on tape
            // Save value in currDisk at currPos - 
            std::cout << "\t" << NeededDisk << "\n";
            DiskString.pop_back(); // Remove found disk from string
        }
        Num++;
    }

    
    //wiringPiSetupGpio();
    //ConsoleLogger logger{};
}
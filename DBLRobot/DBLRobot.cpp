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
        DiskString = DiskBinary(Num);
        length = DiskString.length();
        std::cout << Num << "\t" << DiskString << "\n";
        while (!DiskString.empty()) {
            NeededPos = 4 + DiskString.length() - length;
            while (RampPos != NeededPos){ // Shift right position under the ramp
                std::cout << "RampPos:\t" << RampPos << '\n';
                if (NeededPos > RampPos) {
                    // Shift conveyor left
                    RampPos++;
                }
                else {
                    // Shift conveyor right
                    RampPos--;
                }
            }
            // Check if currPos is correct
            // If not shift to right position
            NeededDisk = DiskString.back(); // Disk needed to form binary number
            std::cout << "\t" << NeededDisk << "\n";

            // Check for Needed Disk on conveyor belt
            
            // If found push on tape
            
            // Save value in currDisk at currPos
            
            DiskString.pop_back(); // Remove found disk from DiskString
        }
        // while 
        Num++;
    }

    
    //wiringPiSetupGpio();
    //ConsoleLogger logger{};
}
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
    for (int i = 0; i <= 7; i++) {
        std::cout << i << "\t" << DiskBinary(i) << "\n";
    }
    //wiringPiSetupGpio();
    //ConsoleLogger logger{};
}
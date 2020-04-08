// DBLRobot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include "ILogger.h"
#include "ConsoleLogger.h"

int main() {
    wiringPiSetupGpio();
    ConsoleLogger logger{};
}
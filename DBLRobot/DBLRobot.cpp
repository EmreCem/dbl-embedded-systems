// DBLRobot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ILogger.h"
#include "ConsoleLogger.h"

void test(ILogger* log);

int main()
{
    wiringPiSetupGpio();

    ConsoleLogger logger{};
    std::string msg = "test";
    logger.LogMessage(msg);
    logger.LogError("test123");
    test(&logger);
    int b;
    std::cin >> b;
}

void test(ILogger* log) {
    log->LogError("oh no");
}
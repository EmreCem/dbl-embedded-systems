#pragma once
#include <string>

class ILogger {
public:
	virtual ~ILogger() = default;
	
	// log any errors that occur
	virtual void LogError(std::string message) = 0;

	// for logging what the robot is doing
	virtual void LogDebug(std::string message) = 0;

	// for logging how many disks the robot has taken
	virtual void LogCount(std::string message) = 0;

	// for logging messages received from the broker
	virtual void LogMessage(std::string message) = 0;
};
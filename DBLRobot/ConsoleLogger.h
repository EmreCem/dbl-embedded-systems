#pragma once
#include "ILogger.h"
#include <iostream>
#include <chrono>
#include "Date.h"

class ConsoleLogger : public ILogger {
public:
	void LogError(std::string message) override;
	void LogDebug(std::string message) override;
	void LogCount(std::string message) override;
	void LogMessage(std::string message) override;
private:
	void Log(std::string&& prefix, std::string message);
};

inline void ConsoleLogger::LogError(std::string message) {
	Log("ERR", message);
}

inline void ConsoleLogger::LogDebug(std::string message) {
	Log("DBG", message);
}

inline void ConsoleLogger::LogCount(std::string message) {
	Log("COUNT", message);
}

inline void ConsoleLogger::LogMessage(std::string message) {
	Log("MSG", message);
}

inline void ConsoleLogger::Log(std::string&& prefix, const std::string message) {
	auto timeNow = std::chrono::system_clock::now();
	auto timeString = date::format("%H:%M:%S", timeNow);
	std::cout << "[" << timeString << "][" << prefix << "] " << message << "\n";
}

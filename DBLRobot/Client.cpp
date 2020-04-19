#include "Client.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std::string_literals;

Client::Client(const char* id, const char* host, int port, ILogger* logger) : mosquittopp(id) {
	int keepalive = DEFAULT_KEEP_ALIVE;
	this->_logger = logger;
	connect(host, port, keepalive);
}

Client::~Client() {
}

void Client::on_connect(int rc) {
	if (!rc) {
		_logger->LogDebug("Connected - code " + rc);
	}
}

void Client::on_subscribe(int mid, int qos_count, const int* granted_qos) {
	_logger->LogDebug("Subscription succeeded.");
}

void Client::on_message(const struct mosquitto_message* message) {
	const int payload_size = 60;
	char buf[payload_size];

	memset(buf, 0, payload_size * sizeof(char));

	/* Copy N-1 bytes to ensure always 0 terminated. */
	if (payload_size == 0) return;
	memcpy(buf, message->payload, MAX_PAYLOAD * sizeof(char));

	std::string convertedMessage(buf);

	if (convertedMessage.length() == 0) {
		return;
	}
	
	_logger->LogMessage(convertedMessage);

	// Examples of messages for M2M communications...
	parse_message(convertedMessage);
}

void Client::parse_message(std::string message) {
	if (message == "start") {
		isEmergency = false;
		shouldStop = false;
		_logger->LogDebug("Starting factory motor and robot's execution.");
		factoryMotor->MoveClockwise();
	}

	if (message == "stop") {
		shouldStop = true;
		_logger->LogDebug("Stopping factory motor and robot's execution.");
		factoryMotor->Stop();
	}

	if (message == "emergencyStop") {
		isEmergency = true;
		_logger->LogDebug("Emergency stop received, pausing robot's execution.");
		factoryMotor->Stop();
	}
	
	if (message == "heartbeat1") {
		robot1Ignored = false;
		_logger->LogDebug("Heartbeat with ID 1 received. Updating last arrived time.");
		heartbeat1LastReceived = millis();
	}

	if (message == "heartbeat2") {
		robot2Ignored = false;
		_logger->LogDebug("Heartbeat with ID 2 received. Updating last arrived time.");
		heartbeat2LastReceived = millis();
	}

	if (message == "heartbeat3") {
		robot3Ignored = false;
		_logger->LogDebug("Heartbeat with ID 3 received. Updating last arrived time.");
		heartbeat3LastReceived = millis();
	}

	if (message == "heartbeat4") {
		robot4Ignored = false;
		_logger->LogDebug("Heartbeat with ID 4 received. Updating last arrived time.");
		heartbeat4LastReceived = millis();
	}

	if (message.substr(0, 8) == "tookDisk") {
		if (message.substr(9, 1) == "1") {
			++robot1Counter;
			_logger->LogDebug("Increasing robot 1 disk counter to " + std::to_string(robot1Counter));
		}

		if (message.substr(9, 1) == "2") {
			++robot2Counter;
			_logger->LogDebug("Increasing robot 2 disk counter to " + std::to_string(robot2Counter));
		}

		if (message.substr(9, 1) == "3") {
			++robot3Counter;
			_logger->LogDebug("Increasing robot 3 disk counter to " + std::to_string(robot3Counter));
		}

		if (message.substr(9, 1) == "4") {
			++robot4Counter;
			_logger->LogDebug("Increasing robot 4 disk counter to " + std::to_string(robot4Counter));
		}
	}

	if (message.substr(0, 5) == "error") {
		if (message.substr(6, 1) == "1") {
			robot1Ignored = true;
			_logger->LogDebug("Robot with id 1 encountered an error.");
		}

		if (message.substr(6, 1) == "2") {
			robot2Ignored = true;
			_logger->LogDebug("Robot with id 2 encountered an error.");
		}

		if (message.substr(6, 1) == "3") {
			robot3Ignored = true;
			_logger->LogDebug("Robot with id 3 encountered an error.");
		}

		if (message.substr(6, 1) == "4") {
			robot4Ignored = true;
			_logger->LogDebug("Robot with id 4 encountered an error.");
		}
	}

	if (message.substr(0, 17) == "requestDisksTaken") {
		if (std::stoi(message.substr(18, 1)) == (robotId - '0')) {
			_logger->LogDebug("requestDisksTaken is from us, ignore.");
			return;
		}

		publish_message(
			"respondDisksTaken:"s + robotId + "," + std::to_string(robot1Counter) + "," + std::to_string(robot2Counter)
			+ ","
			+ std::to_string(robot3Counter) + "," + std::to_string(robot4Counter));
	}

	if (message.substr(0, 17) == "respondDisksTaken") {
		/*if (disksTakenRequested) {
			int counter1 = std::stoi(message.substr(18, 1));
			int counter2 = std::stoi(message.substr(20, 1));
			int counter3 = std::stoi(message.substr(22, 1));
			int counter4 = std::stoi(message.substr(24, 1));
		}*/

		disksTakenRequested = false;
	}
}

void Client::publish_message(std::string message) {
	char buf[50];
	message.copy(buf, message.size());
	buf[message.size()] = '\0';
	publish(NULL, outTopic, strlen(buf), buf);
	std::stringstream ss{};
	ss << "Sent \"" << message << "\" to " << std::string(outTopic);
	_logger->LogDebug(ss.str());
}

void Client::main_loop(char id, MotorController* factoryBeltMotor) {
	factoryMotor = factoryBeltMotor;
	int rc = 0;
	bool notSubbed = true;
	unsigned int timeNow = millis();

	std::string topicIn = "factory/robotx/in";
	topicIn[13] = id;

	std::string topicOut = "factory/robotx/out";
	topicOut[13] = id;

	_logger->LogDebug(topicIn);
	_logger->LogDebug(topicOut);

	mosqpp::lib_init();
	outTopic = topicOut.c_str();

	_logger->LogDebug("Setting out topic to: " + std::string(outTopic));
	robotId = id;

	while (true) {
		rc = loop();

		if (rc) {
			reconnect();
		} else {
			if (notSubbed) {
				subscribe(NULL, topicIn.c_str());
				notSubbed = false;
			}
		}

		if (millis() - timeNow > 5000) {
			// according to the protocol send a heartbeat every 5s
			publish_message(std::string("heartbeat") + robotId);
			timeNow = millis();
		}

		check_heartbeats();
	}

	mosqpp::lib_cleanup();
}

void Client::check_heartbeats() {
	if (heartbeat1LastReceived != 0 && millis() - heartbeat1LastReceived > 60000) {
		_logger->LogDebug("Robot 1 has died.");
		heartbeat1LastReceived = 0;
		robot1Ignored = true;
	}

	if (heartbeat2LastReceived != 0 && millis() - heartbeat2LastReceived > 60000) {
		_logger->LogDebug("Robot 2 has died.");
		heartbeat2LastReceived = 0;
		robot2Ignored = true;
	}

	if (heartbeat3LastReceived != 0 && millis() - heartbeat3LastReceived > 60000) {
		_logger->LogDebug("Robot 3 has died.");
		heartbeat3LastReceived = 0;
		robot3Ignored = true;
	}

	if (heartbeat4LastReceived != 0 && millis() - heartbeat4LastReceived > 60000) {
		_logger->LogDebug("Robot 4 has died.");
		heartbeat4LastReceived = 0;
		robot4Ignored = true;
	}
}

void Client::diskTaken() {
	publish_message(std::string("tookDisk:") + robotId);
	ownCounter++;
}

bool Client::allowedToTakeDisk() {
	std::vector<int> counters{};
	if (!robot1Ignored) {
		counters.push_back(robot1Counter);
	}
	if (!robot2Ignored) {
		counters.push_back(robot2Counter);
	}
	if (!robot3Ignored) {
		counters.push_back(robot3Counter);
	}
	if (!robot4Ignored) {
		counters.push_back(robot4Counter);
	}
	
	int minElement = *std::min_element(counters.begin(), counters.end());
	
	if (ownCounter < minElement + 3) {
		_logger->LogDebug("Our counter: " + std::to_string(ownCounter) + ", min counter: " + std::to_string(minElement) + ", allowed to take disk.");
		return true;
	}

	_logger->LogDebug("Our counter: " + std::to_string(ownCounter) + ", min counter: " + std::to_string(minElement) + ", NOT allowed to take disk.");
	return false;
}

void Client::requestDisksTaken() {
	disksTakenRequested = true;
	publish_message(std::string("requestDisksTaken:") + robotId);
}

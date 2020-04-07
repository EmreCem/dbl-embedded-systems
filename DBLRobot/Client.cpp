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
    memcpy(buf, message->payload, MAX_PAYLOAD * sizeof(char));

    std::string convertedMessage(buf);
    _logger->LogMessage(convertedMessage);

    // Examples of messages for M2M communications...
    parse_message(convertedMessage);
}

void Client::parse_message(std::string message) {
    if (message == "heartbeat1") {
        _logger->LogDebug("Heartbeat with ID 1 received. Updating last arrived time.");
        heartbeat1LastReceived = millis();
    }

    if (message == "heartbeat2") {
        _logger->LogDebug("Heartbeat with ID 2 received. Updating last arrived time.");
        heartbeat2LastReceived = millis();
    }

    if (message == "heartbeat3") {
        _logger->LogDebug("Heartbeat with ID 3 received. Updating last arrived time.");
        heartbeat3LastReceived = millis();
    }

    if (message == "heartbeat4") {
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

    if (message.substr(0, 17) == "requestDisksTaken") {
        publish_message("respondDisksTaken:"s + robotId + "," + std::to_string(robot1Counter) + "," + std::to_string(robot2Counter) + ","
            + std::to_string(robot3Counter) + "," + std::to_string(robot4Counter));
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

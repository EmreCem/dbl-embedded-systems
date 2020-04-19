#pragma once

#include <mosquittopp.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <wiringPi.h>
#include "ConsoleLogger.h"
#include "MotorController.h"
#include <vector>
#include <atomic>

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class Client : public mosqpp::mosquittopp {
public:
    Client(const char* id, const char* host, int port, ILogger* logger);
    ~Client();

    void on_connect(int rc);
    void on_message(const struct mosquitto_message* message);
    void on_subscribe(int mid, int qos_count, const int* granted_qos);
    void parse_message(std::string message);
    void publish_message(std::string message);
    void main_loop(char, MotorController*);
    void check_heartbeats();
    void diskTaken();
    bool allowedToTakeDisk();
    void requestDisksTaken();

    unsigned int heartbeat1LastReceived = 0;
    unsigned int heartbeat2LastReceived = 0;
    unsigned int heartbeat3LastReceived = 0;
    unsigned int heartbeat4LastReceived = 0;

    bool robot1Ignored = true;
    bool robot2Ignored = true;
    bool robot3Ignored = true;
    bool robot4Ignored = true;

    int robot1Counter = 0;
    int robot2Counter = 0;
    int robot3Counter = 0;
    int robot4Counter = 0;
    int ownCounter = 0;

    const char* outTopic;
    char robotId;
    MotorController* factoryMotor;
    bool shouldStop = false;
    bool isEmergency = false;
    bool disksTakenRequested = false;
private:
    ILogger* _logger;
};


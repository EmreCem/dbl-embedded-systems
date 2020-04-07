#include "Client.h"
#include "ConsoleLogger.h"
#include <iostream>
#include <sstream>

ConsoleLogger logger{};
Client client("robot", "test.mosquitto.org", 1883, &logger);

void checkHeartbeats() {
    if (client.heartbeat1LastReceived != 0 && millis() - client.heartbeat1LastReceived > 60000) {
        logger.LogDebug("Robot 1 has died.");
        client.heartbeat1LastReceived = 0;
    }

    if (client.heartbeat2LastReceived != 0 && millis() - client.heartbeat2LastReceived > 60000) {
        logger.LogDebug("Robot 2 has died.");
        client.heartbeat2LastReceived = 0;
    }

    if (client.heartbeat3LastReceived != 0 && millis() - client.heartbeat3LastReceived > 60000) {
        logger.LogDebug("Robot 3 has died.");
        client.heartbeat3LastReceived = 0;
    }

    if (client.heartbeat4LastReceived != 0 && millis() - client.heartbeat4LastReceived > 60000) {
        logger.LogDebug("Robot 4 has died.");
        client.heartbeat4LastReceived = 0;
    }
}

int main() {
    int rc = 0;
    bool notSubbed = true;
    char robotId;
    unsigned int timeNow = millis();

    std::cout << "Please enter the robot's ID: ";
    std::cin >> robotId;

    std::ostringstream ss{};
    ss << "factory/robot" << robotId << "/in";
    auto topicIn = ss.str();

    ss.str(std::string());
    ss << "factory/robot" << robotId << "/out";
    auto topicOut = ss.str();

    logger.LogDebug(topicIn);
    logger.LogDebug(topicOut);

    mosqpp::lib_init();
    client.outTopic = topicOut.c_str();
    client.robotId = robotId;

    while (true) {
        rc = client.loop();

        if (rc) {
            client.reconnect();
        } else {
            if (notSubbed) {
                client.subscribe(NULL, topicIn.c_str());
                notSubbed = false;
            }
        }

        if (millis() - timeNow > 5000) {
            client.publish_message(std::string("heartbeat") + robotId);
            timeNow = millis();
        }

        checkHeartbeats();
    }

    mosqpp::lib_cleanup();
    return 1;
}

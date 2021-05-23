#pragma once

#include <string>

class SystemConfig {

public:
    // System
    std::string logLevel;
    std::string logFile;
    int endpointPort{};
    int endpointNumThreads{};

    SystemConfig();

    void print() const;

    void reset();

private:
    void initialise();
};

SystemConfig &getSystemConfig();

#include <utils/config.h>
#include <faabric/util/environment.h>
#include <faabric/util/locks.h>
#include <utils/logging.h>

using namespace faabric;
using namespace faabric::util;

SystemConfig &getSystemConfig() {
    static SystemConfig conf;
    return conf;
}

SystemConfig::SystemConfig() {
    this->initialise();
}

void SystemConfig::initialise() {
    // System
    logLevel = getEnvVar("LOG_LEVEL", "info");
    logFile = getEnvVar("LOG_FILE", "off");
}

void SystemConfig::reset() {
    this->initialise();
}

void SystemConfig::print() const {
    const std::shared_ptr<spdlog::logger> &logger = getLogger();
    logger->info("--- System ---");
    logger->info("LOG_LEVEL                  {}", logLevel);
    logger->info("LOG_FILE                   {}", logFile);
}

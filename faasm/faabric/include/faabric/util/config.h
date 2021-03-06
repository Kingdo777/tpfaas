#pragma once

#include <string>

#define MPI_HOST_STATE_LEN 20

#define DEFAULT_TIMEOUT 60000
#define RESULT_KEY_EXPIRY 30000
#define STATUS_KEY_EXPIRY 300000

namespace faabric::util {
class SystemConfig
{

  public:
    // System
    std::string hostType;
    std::string functionStorage;
    std::string fileserverUrl;
    std::string serialisation;
    std::string cgroupMode;
    std::string netNsMode;
    std::string logLevel;
    std::string logFile;
    std::string pythonPreload;
    std::string captureStdout;
    std::string stateMode;
    std::string wasmVm;
    std::string deltaSnapshotEncoding;

    // Redis
    std::string redisStateHost;
    std::string redisQueueHost;
    std::string redisPort;

    // Scheduling
    int noScheduler;
    int overrideCpuCount;

    // Worker-related timeouts
    int globalMessageTimeout;
    int unboundTimeout;
    int boundTimeout;
    int chainedCallTimeout;

    // Filesystem storage
    std::string functionDir;
    std::string objectFileDir;
    std::string runtimeFilesDir;
    std::string sharedFilesDir;
    std::string sharedFilesStorageDir;

    // MPI
    int defaultMpiWorldSize;

    // Endpoint
    std::string endpointInterface;
    std::string endpointHost;
    int endpointPort;
    int endpointNumThreads;

    SystemConfig();

    void print();

    void reset();

  private:
    int getSystemConfIntParam(const char* name, const char* defaultValue);

    void initialise();
};

SystemConfig& getSystemConfig();
}

#include <faabric/executor/FaabricMain.h>
#include <faabric/util/config.h>
#include <faabric/util/logging.h>

namespace faabric::executor {
FaabricMain::FaabricMain(faabric::executor::FaabricPool& poolIn)
  : conf(faabric::util::getSystemConfig())
  , scheduler(faabric::scheduler::getScheduler())
  , pool(poolIn)
{}

// 旨在启动schedule和pool的各种服务
void FaabricMain::startBackground()
{
    // 将本地的host写到全局的redis缓存中
    scheduler.addHostToGlobalSet();

    conf.print();

    // Start thread pool in background
    pool.startThreadPool();

    // 下面三个是grpc的服务器端，也就是微服务

    // In-memory state
    pool.startStateServer();

    // Snapshots
    pool.startSnapshotServer();

    // Work sharing
    pool.startFunctionCallServer();
}

void FaabricMain::shutdown()
{
    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();
    logger->info("Removing from global working set");

    scheduler.shutdown();

    pool.shutdown();
}
}

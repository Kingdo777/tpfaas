#include <faabric/executor/FaabricExecutor.h>
#include <faabric/executor/FaabricPool.h>
#include <faabric/util/logging.h>

namespace faabric::executor {
FaabricPool::FaabricPool(int nThreads)
  : _shutdown(false)
  , scheduler(faabric::scheduler::getScheduler())
  , threadTokenPool(nThreads)
  , stateServer(faabric::state::getGlobalState())
{

    // Ensure we can ping both redis instances
    faabric::redis::Redis::getQueue().ping();
    faabric::redis::Redis::getState().ping();
}

void FaabricPool::startFunctionCallServer()
{
    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();
    logger->info("Starting function call server");
    functionServer.start();
}
// 看起来目前并没有实现snapshotServer
// schedule中的确在调用function的时候是没有用到这个功能的
void FaabricPool::startSnapshotServer()
{
    auto logger = faabric::util::getLogger();
    logger->info("Starting snapshot server");
}

void FaabricPool::startStateServer()
{
    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();

    // Skip state server if not in inmemory mode
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
    if (conf.stateMode != "inmemory") {
        logger->info("Not starting state server in state mode {}",
                     conf.stateMode);
        return;
    }

    // Note that the state server spawns its own background thread
    logger->info("Starting state server");
    stateServer.start();
}

// 启动线程池
// background,是否在后台运行，默认是ture
void FaabricPool::startThreadPool(bool background)
{
    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();
    logger->info("Starting executor thread pool");

    // Spawn worker threads until we've hit the worker limit, thus creating a
    // pool that will replenish when one releases its token
    // poolThead是一个派生线程，用来负责创建线程池的，将根据ThreadToken来维护池
    poolThread = std::thread([this] {
        const std::shared_ptr<spdlog::logger>& logger =
          faabric::util::getLogger();
        // 只要没有shutdown，那么就循环执行
        while (!this->isShutdown()) {
            // Try to get an available slot (blocks if none available)
            // 首先获取执行token，token的个数是在构建pool的时候指定的
            // 如果token已经全部被take，那么这条指令将会阻塞（通过queue实现的阻塞）
            int threadIdx = this->getThreadToken();

            // Double check shutdown condition
            if (this->isShutdown()) {
                break;
            }

            // Spawn thread to execute function
            // 创建线程池，每个线程都将创建一个executor,然后执行run命令
            // 当run执行结束，这个线程也就终止了，将会在终止前释放token
            // 他的逻辑是这样的：
            // 首先根据线程池的大小创建并维护一定数目的线程
            // 这些线程，一开始都是未绑定的状态，创建之后都会去读bingQueue，如果等到一个绑定请求，就和这个function绑定
            // 如果一直没有等到，那就超时,超时后将从run返回
            // 一旦绑定请求，将去读queueMap中所绑定的函数的请求队列，以处理请求
            // 直到队列为空，超时后run结束
            // 这也是基类的核心逻辑，派生类如fasslet主要将完成do_bind和do_execute的工作
            poolThreads.emplace_back(std::thread([this, threadIdx] {
                std::unique_ptr<FaabricExecutor> executor =
                  createExecutor(threadIdx);

                // Worker will now run for a long time
                try {
                    executor->run();
                } catch (faabric::executor::ExecutorPoolFinishedException& e) {
                    //一旦一个线程出问题，那么将直接导致派生线程结束执行
                    this->_shutdown = true;
                }

                // Handle thread finishing
                threadTokenPool.releaseToken(executor->threadIdx);
            }));
        }

        // Once shut down, wait for everything to die
        logger->info("Waiting for {} worker threads", poolThreads.size());
        for (auto& t : poolThreads) {
            if (t.joinable()) {
                t.join();
            }
        }

        // Will die gracefully at this point
    });

    // Block if in foreground
    // 如果不是后台运行，那么讲一直等待派生线程结束
    if (!background) {
        poolThread.join();
    }
}

void FaabricPool::reset()
{
    threadTokenPool.reset();
}

int FaabricPool::getThreadToken()
{
    return threadTokenPool.getToken();
}

int FaabricPool::getThreadCount()
{
    return threadTokenPool.taken();
}

bool FaabricPool::isShutdown()
{
    return _shutdown;
}

void FaabricPool::shutdown()
{
    _shutdown = true;

    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();

    logger->info("Waiting for the state server to finish");
    stateServer.stop();

    logger->info("Waiting for the function server to finish");
    functionServer.stop();

    if (poolThread.joinable()) {
        logger->info("Waiting for pool to finish");
        poolThread.join();
    }

    if (mpiThread.joinable()) {
        logger->info("Waiting for mpi thread to finish");
        mpiThread.join();
    }

    logger->info("Faabric pool successfully shut down");
}
}

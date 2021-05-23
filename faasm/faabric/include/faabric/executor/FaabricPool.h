#pragma once

#include <faabric/executor/FaabricExecutor.h>
#include <faabric/scheduler/FunctionCallServer.h>
#include <faabric/scheduler/Scheduler.h>
#include <faabric/state/StateServer.h>
#include <faabric/util/queue.h>

namespace faabric::executor {
class FaabricPool
{
  public:
    explicit FaabricPool(int nThreads);

    void startFunctionCallServer();

    void startThreadPool(bool background = true);

    void startStateServer();

    void startSnapshotServer();

    void reset();

    int getThreadToken();

    int getThreadCount();

    bool isShutdown();

    void shutdown();

  protected:
    virtual std::unique_ptr<FaabricExecutor> createExecutor(int threadIdx) = 0;

  private:
    std::atomic<bool> _shutdown = false;
    faabric::scheduler::Scheduler& scheduler;
    // TokenPool是通过queue实现的，用于控制线程的个数，只有拿到token才能创建新的线程
    faabric::util::TokenPool threadTokenPool;
    // state微服务器
    faabric::state::StateServer stateServer;
    // Function微服务器，用于在其他host上调用函数
    faabric::scheduler::FunctionCallServer functionServer;

    std::thread mpiThread;
    // 用于创建县城池的派生线程
    std::thread poolThread;
    // 线程池
    std::vector<std::thread> poolThreads;
};

class ExecutorPoolFinishedException : public faabric::util::FaabricException
{
  public:
    explicit ExecutorPoolFinishedException(std::string message)
      : FaabricException(std::move(message))
    {}
};
}

// Macro for quickly defining functions
#define FAABRIC_EXECUTOR()                                                     \
                                                                               \
    bool _execFunc(faabric::Message& msg);                                     \
                                                                               \
    class _Executor final : public FaabricExecutor                             \
    {                                                                          \
      public:                                                                  \
        explicit _Executor(int threadIdx)                                      \
          : FaabricExecutor(threadIdx)                                         \
        {}                                                                     \
                                                                               \
        bool doExecute(faabric::Message& msg) override                         \
        {                                                                      \
            return _execFunc(msg);                                             \
        }                                                                      \
    };                                                                         \
    class _Pool : public FaabricPool                                           \
    {                                                                          \
      public:                                                                  \
        explicit _Pool(int nThreads)                                           \
          : FaabricPool(nThreads)                                              \
        {}                                                                     \
                                                                               \
        std::unique_ptr<FaabricExecutor> createExecutor(                       \
          int threadIdx) override                                              \
        {                                                                      \
            return std::make_unique<_Executor>(threadIdx);                     \
        }                                                                      \
    };                                                                         \
    bool _execFunc(faabric::Message& msg)

#pragma once

#include <faabric/proto/faabric.pb.h>

#include <faabric/scheduler/Scheduler.h>
#include <faabric/util/logging.h>
// 我们创建的线程池，都会创建一个executor，然后调用其run函数
namespace faabric::executor {
class FaabricExecutor
{
  public:
    explicit FaabricExecutor(int threadIdxIn);

    virtual ~FaabricExecutor() {}

    void bindToFunction(const faabric::Message& msg, bool force = false);

    void run();

    bool isBound();

    virtual std::string processNextMessage();

    std::string executeCall(faabric::Message& call);

    void finish();

    virtual void flush();

    std::string id;
    // 派生线池创建线程池的线程的时候获取的tokens
    const int threadIdx;

  protected:
    virtual bool doExecute(faabric::Message& msg);

    virtual void postBind(const faabric::Message& msg, bool force);

    virtual void preFinishCall(faabric::Message& call,
                               bool success,
                               const std::string& errorMsg);

    virtual void postFinishCall();

    virtual void postFinish();

    bool _isBound = false;

    faabric::scheduler::Scheduler& scheduler;

    std::shared_ptr<faabric::scheduler::InMemoryMessageQueue> currentQueue;

    int executionCount = 0;

  private:
    faabric::Message boundMessage;

    void finishCall(faabric::Message& msg,
                    bool success,
                    const std::string& errorMsg);
};
}

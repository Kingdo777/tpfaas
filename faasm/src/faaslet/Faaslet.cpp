#include "Faaslet.h"

#include <stdexcept>
#include <system/CGroup.h>
#include <system/NetworkNamespace.h>

#include <faabric/scheduler/Scheduler.h>
#include <faabric/util/config.h>
#include <faabric/util/locks.h>
#include <module_cache/WasmModuleCache.h>

#include <wavm/WAVMWasmModule.h>

#include <storage/FileLoader.h>
#include <storage/FileSystem.h>

using namespace isolation;

namespace faaslet {

std::mutex flushMutex;

void preloadPythonRuntime()
{
    auto logger = faabric::util::getLogger();

    auto& conf = faabric::util::getSystemConfig();
    if (conf.pythonPreload != "on") {
        logger->info("Not preloading python runtime");
        return;
    }

    logger->info("Preparing python runtime");

    faabric::Message msg =
      faabric::util::messageFactory(PYTHON_USER, PYTHON_FUNC);
    msg.set_ispython(true);
    msg.set_pythonuser("python");
    msg.set_pythonfunction("noop");
    faabric::util::setMessageId(msg);

    faabric::scheduler::Scheduler& sch = faabric::scheduler::getScheduler();
    sch.callFunction(msg, true);
}

void Faaslet::flush()
{
    auto logger = faabric::util::getLogger();
    logger->debug("Faaslet {} flushing", id);

    // Note that all warm Faaslets on the given host will be flushing at the
    // same time, so we need to include some locking.
    // TODO avoid repeating global tidy-up that only needs to be done once
    faabric::util::UniqueLock lock(flushMutex);

    // Clear cached shared files
    storage::FileSystem::clearSharedFiles();

    // Clear cached wasm and object files
    storage::FileLoader& fileLoader = storage::getFileLoader();
    fileLoader.flushFunctionFiles();

    // Flush the module itself
    if (_isBound) {
        module->flush();
    }

    // Clear module cache on this host
    module_cache::getWasmModuleCache().clear();

    // Terminate this Faaslet
    throw faabric::util::ExecutorFinishedException("Faaslet flushed");
}

// fasslet创建时要设置Cgroup和NS
Faaslet::Faaslet(int threadIdxIn)
  : FaabricExecutor(threadIdxIn)
{
    // Set up network namespace
    isolationIdx = threadIdx + 1;
    std::string netnsName = BASE_NETNS_NAME + std::to_string(isolationIdx);
    ns = std::make_unique<NetworkNamespace>(netnsName);
    ns->addCurrentThread();

    // Add this thread to the cgroup
    CGroup cgroup(BASE_CGROUP_NAME);
    cgroup.addCurrentThread();
}
// 在推出时记得从NS中移除
void Faaslet::postFinish()
{
    ns->removeCurrentThread();
}

// 到这里请求已经执行完毕，这段代码在finishCall的最开始执行，而postFinish在最后执行
void Faaslet::preFinishCall(faabric::Message& call,
                            bool success,
                            const std::string& errorMsg)
{
    const std::shared_ptr<spdlog::logger>& logger = faabric::util::getLogger();
    const std::string funcStr = faabric::util::funcToString(call, true);

    // Add captured stdout if necessary
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
    if (conf.captureStdout == "on") {
        std::string moduleStdout = module->getCapturedStdout();
        if (!moduleStdout.empty()) {
            std::string newOutput = moduleStdout + "\n" + call.outputdata();
            call.set_outputdata(newOutput);

            module->clearCapturedStdout();
        }
    }

    logger->debug("Resetting module {} from zygote", funcStr);
    // Restore from zygote
    wasm::WAVMWasmModule &cachedModule =
            module_cache::getWasmModuleCache().getCachedModule(call);
    module = std::make_unique<wasm::WAVMWasmModule>(cachedModule);
}

// 核心逻辑依然是bind-exec-rebind
// 直接获取module对象，然后调用exec，执行完毕之后回复module
void Faaslet::postBind(const faabric::Message& msg, bool force)
{
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
    auto logger = faabric::util::getLogger();
    std::string funcStr = faabric::util::funcToString(msg, false);

    // Get cached module
    wasm::WAVMWasmModule &cachedModule =
            module_cache::getWasmModuleCache().getCachedModule(msg);
    module = std::make_unique<wasm::WAVMWasmModule>(cachedModule);
}

bool Faaslet::doExecute(faabric::Message& msg)
{
    auto logger = faabric::util::getLogger();

    auto& conf = faabric::util::getSystemConfig();
    const std::string snapshotKey = msg.snapshotkey();

    // Execute the function
    bool success = module->execute(msg);
    return success;
}
}

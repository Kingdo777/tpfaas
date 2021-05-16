//
// Created by kingdo on 10/17/20.
//

#include <utility>
#include "RFIT/F/function.h"
#include "RFIT/R/resource.h"
#include "utils/logging.h"


// 借助WAVM的HashMap工具，定义一个全局的Function的表
struct FunctionCache {
public:
    Platform::RWMutex mutex;
    HashMap<string, F *> functionMap;

    static FunctionCache &get() {
        static FunctionCache fc;
        return fc;
    }

private:
    FunctionCache() = default;

    ~FunctionCache() {
        for (auto i = functionMap.begin(); i < functionMap.end(); ++i) {
            delete i->value;
        }
        functionMap.clear();
    }
};

F::F(string &&functionName, void (*entry_addr)(FCGX_Request *), Resource &&res, Uptr concurrencyLimit) :
        name(functionName), entry_addr(entry_addr), r(R::getUniqueR(std::move(res))),
        concurrencyLimit(concurrencyLimit) {
    r->addF(this);
    getLogger()->info("register A new F : <" + this->toString() + ">");
}

F *
F::getF(string &&functionName) {
    FunctionCache &fc = FunctionCache::get();
    Platform::RWMutex::ShareableLock shareableLock(fc.mutex);
    F **f = fc.functionMap.get(functionName);
    return f ? *f : nullptr;
}

void
F::registerF(string &&functionName, void (*entry_addr)(FCGX_Request *), Resource &&res, Uptr concurrencyLimit) {
    FunctionCache &fc = FunctionCache::get();
    {
        Platform::RWMutex::ShareableLock shareableLock(fc.mutex);
        F **f = fc.functionMap.get(functionName);
        if (f)
            return;
    }
    WAVM_ASSERT(entry_addr)
    Platform::RWMutex::ExclusiveLock exclusiveLock(fc.mutex);
    F *&f = fc.functionMap.getOrAdd(functionName, nullptr);
    if (f)
        return;
    f = new F(std::move(functionName), entry_addr, std::move(res), concurrencyLimit);
}

bool F::tryScaleDownCap(Iptr scaleCount) {
    WAVM_ASSERT(scaleCount >= 0)
    WAVM_ASSERT (isConcurrency())
    faabric::util::UniqueLock lock(restCap.lock);
    if (restCap.count < scaleCount)
        return false;
    restCap.count -= scaleCount;
    WAVM_ASSERT(restCap.count >= 0)
    return true;
}

Uptr F::scaleCap(Iptr scaleCount) {
    WAVM_ASSERT(isConcurrency())
    faabric::util::UniqueLock lock(restCap.lock);
    restCap.count += scaleCount;
    WAVM_ASSERT(restCap.count >= 0)
    return restCap.count;
}
//
// Created by kingdo on 10/17/20.
//

#ifndef TPFAAS_FUNCTION_H
#define TPFAAS_FUNCTION_H

#include <fcgiapp.h>
#include <utility>
#include <WAVM/Inline/HashMap.h>
#include <WAVM/Platform/RWMutex.h>
#include "RFIT/R/resource.h"

using namespace std;
using namespace WAVM;
using namespace WAVM::Platform;

//调用某个function
#define FUN(f, arg) ((void *(*)(void *))(get_func(f)->entry_addr))(arg);

class I;

///结构体只能向下加参数，不然gogo会出问题
class F {
public:
    static F *getF(string &&functionName);

    static void
    registerF(string &&functionName,
              void (*entry_addr)(FCGX_Request *),
              Resource &&res = Resource(),
              Uptr concurrencyLimit = 1);

    void addI2F(I *i) {
        faabric::util::UniqueLock lock(globalIQueue.getMx());
        globalIQueue.enqueue(i);
    };

    void tryGetMultiI(vector<I *> &iVector, Uptr count) {
        Uptr iCount = globalIQueue.size();
        if (iCount == 0 || count == 0)
            return;
        faabric::util::UniqueLock lock(globalIQueue.getMx());
        iCount = globalIQueue.size();
        if (iCount == 0)
            return;
        Uptr pull_count = count < iCount ? count : iCount;
        for (Uptr j = 0; j < pull_count; ++j) {
            iVector.push_back(globalIQueue.tryDequeue(nullptr));
        }
    }

    Uptr getICount() {
        faabric::util::UniqueLock lock(globalIQueue.getMx());
        return globalIQueue.size();
    };

    [[nodiscard]] const Queue<I *> &getGlobalIQueue() const {
        return globalIQueue;
    }

    Uptr scaleCap(Iptr scaleCount);

    bool tryScaleDownCap(Iptr scaleCount = 1);

    [[nodiscard]] bool isConcurrency() const { return concurrencyLimit > 1; };

    [[nodiscard]] Uptr getConcurrencyLimit() const { return concurrencyLimit; };

    R *getR() { return r; }

    auto getEntryAddr() {
        return entry_addr;
    }


    [[nodiscard]] const string &getName() const {
        return name;
    }

    string toString() {
        return "F::" + name + "::C-" + to_string(concurrencyLimit);
    }

private:
    const string name;

    // function的入口地址
    void (*entry_addr)(FCGX_Request *);

    // 资源限制
    R *r;
    // 并发限制
    Uptr concurrencyLimit = 1;

    Queue<I *> globalIQueue;

    struct Cap {
        mutex lock;
        Iptr count;
    };
    Cap restCap;

    F(string &&function_name,
      void (*entry_addr)(FCGX_Request *),
      Resource &&res = Resource(),
      Uptr concurrencyLimit = 1);
};

#endif //TPFAAS_FUNCTION_H

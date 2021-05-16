//
// Created by kingdo on 10/18/20.
//

#ifndef TPFAAS_RESOURCE_H
#define TPFAAS_RESOURCE_H

#include "RFIT/T/task.h"
#include <WAVM/Inline/HashMap.h>
#include <WAVM/Platform/RWMutex.h>
#include <faabric/util/locks.h>
#include <utils/queue.h>
#include <memory>
#include <utility>
#include <vector>

using namespace std;
using namespace WAVM;

#define CPU_DEFAULT_SHARES 1024
#define CPU_DEFAULT_CFS_QUOTA_US -1
#define CPU_DEFAULT_CFS_PERIOD_US 100000

#define MEM_DEFAULT_SOFT_LIMIT -1
#define MEM_DEFAULT_HARD_LIMIT -1

class F;

class T;

struct CpuResource {
public:
    CpuResource() :
            impl(Impl()) {};

    CpuResource(Uptr cpu_shares, Iptr cfs_quota_us, Iptr cfs_period_us) :
            impl(Impl(cpu_shares, cfs_quota_us, cfs_period_us)) {};

    [[nodiscard]] Uptr getHash() const { return impl.hash; }

    friend bool operator==(const CpuResource &left, const CpuResource &right) {
        return left.impl.hash == right.impl.hash;
    }

    friend bool operator!=(const CpuResource &left, const CpuResource &right) {
        return left.impl.hash != right.impl.hash;
    }

    [[nodiscard]] string toString() const {
        return "CpuResource::cpu_shares-" + to_string(impl.cpuShares) + "::cfs_quota_us-" +
               to_string(impl.cfs_quota_us) + "::cfs_period_us-" + to_string(impl.cfs_period_us);
    }

private:
    struct Impl {
        Uptr hash;
        Uptr cpuShares;
        Iptr cfs_quota_us;
        Iptr cfs_period_us;

        explicit Impl(Uptr cpu_shares = CPU_DEFAULT_SHARES, Iptr cfs_quota_us = CPU_DEFAULT_CFS_QUOTA_US,
                      Iptr cfs_period_us = CPU_DEFAULT_CFS_PERIOD_US);
    };

    const Impl impl;
};

struct MemResource {
public:
    MemResource() :
            impl(Impl()) {};

    MemResource(Iptr mem_soft_limit, Iptr mem_hard_limit) :
            impl(Impl(mem_soft_limit, mem_hard_limit)) {};

    [[nodiscard]] Uptr getHash() const { return impl.hash; }

    friend bool operator==(const MemResource &left, const MemResource &right) {
        return left.impl.hash == right.impl.hash;
    }

    friend bool operator!=(const MemResource &left, const MemResource &right) {
        return left.impl.hash != right.impl.hash;
    }

    [[nodiscard]] string toString() const {
        return "MemResource::mem_soft_limit-" + to_string(impl.mem_soft_limit) + "::mem_hard_limit-" +
               to_string(impl.mem_hard_limit);
    }

private:
    struct Impl {
        Uptr hash;
        Iptr mem_soft_limit;
        Iptr mem_hard_limit;

        // 就一个构造函数,而且此构造函数，仅仅在getUniqueImpl中被调用，这样的话，所有的Impl就会保证是唯一的
        explicit Impl(Iptr mem_soft_limit = MEM_DEFAULT_SOFT_LIMIT, Iptr mem_hard_limit = MEM_DEFAULT_HARD_LIMIT);
    };

    const Impl impl;
};

struct Resource {
    Resource() :
            impl(Impl()) {};

    Resource(CpuResource cpu,
             MemResource mem) :
            impl(Impl(cpu, mem)) {};

    [[nodiscard]] Uptr getHash() const { return impl.hash; }

    [[nodiscard]] string toString() const {
        return "Resource::{" + impl.cpu.toString() + "," + impl.mem.toString() + "}";
    }

private:
    struct Impl {
        Uptr hash;
        CpuResource cpu;
        MemResource mem;

        // 就一个构造函数,而且此构造函数，仅仅在getUniqueImpl中被调用，这样的话，所有的Impl就会保证是唯一的
        explicit Impl(CpuResource cpu = CpuResource(), MemResource mem = MemResource());
    };

    const Impl impl;
};


class R {
public:
    static R *getUniqueR(Resource &&res = Resource());

    static Uptr getRCount();

    Uptr getFCount() { return functions.size(); };

    void addF(F *f) { functions.push_back(f); };

    static T *getIdleT(R *r);

    T *getIdleTFromOtherR();

    T *getTFromIdle();

    void putT2Idle(T *t);

    void removeTFromBusy(T *t);

    void putT2Busy(T *t);

    [[nodiscard]] const vector<F *> &getFunctions() const {
        return functions;
    }

    [[nodiscard]]  mutex &getBusyTaskLock() {
        return busyTask.lock;
    }

    [[nodiscard]] const HashMap<Uptr, T *> &getBusyTask() const {
        return busyTask.map;
    }

    Uptr getBusyTaskCount_safe() {
        faabric::util::UniqueLock lock(busyTask.lock);
        return busyTask.map.size();
    }

    [[nodiscard]] string toString() const {
        return "R::" + r.toString();
    }

private:
    const Resource r;

    // 在此R上的所有F
    vector<F *> functions;

    explicit R(Resource r = Resource()) : r(std::move(r)) {};

    Queue<T *> idleTask;

    struct BusyTask {
        mutex lock;
        WAVM::HashMap<Uptr, T *> map;
    };
    BusyTask busyTask;
};

#endif //TPFAAS_RESOURCE_H

//
// Created by kingdo on 2020/10/23.
//
#include <faabric/util/locks.h>
#include <utils/sync.h>
#include "RFIT/R/resource.h"

// 借助WAVM的HashMap工具，定义一个全局的Function的表
struct ResourceCache {
    Platform::RWMutex mutex;
    HashMap<Uptr, R *> resourceMap;

    static ResourceCache &get() {
        static ResourceCache rc;
        return rc;
    }

    static void reset() {
        ResourceCache &rc = ResourceCache::get();
        Platform::RWMutex::ExclusiveLock exclusiveLock(rc.mutex);
        rc.resourceMap.clear();
    }

private:
    ResourceCache() = default;

    ~ResourceCache() {
        for (auto i = resourceMap.begin(); i < resourceMap.end(); ++i) {
            delete (i->value);
        }
        resourceMap.clear();
    }
};


R *R::getUniqueR(Resource &&res) {
    ResourceCache &rc = ResourceCache::get();
    {
        // 在这里加的共享锁，加了以后其他线程可以再加共享锁，但是不能加排他锁
        Platform::RWMutex::ShareableLock shareableLock(rc.mutex); // shareableLock是读锁，又叫共享锁
        R **r = rc.resourceMap.get(res.getHash());
        if (r)
            return *r;
    }
    // 在这里进行了锁升级，获取了排他锁，这样其他线程将无法获得锁
    // 获得的锁，最后都将在析构函数中释放
    Platform::RWMutex::ExclusiveLock exclusiveLock(rc.mutex); // exclusiveLock是写锁，又叫排它锁
    R *&r = rc.resourceMap.getOrAdd(res.getHash(), nullptr);
    if (r)
        return r;
    // 创建新的对象
    R *r_tmp = new R(res);
    // 将对象指针放入map
    r = r_tmp;
    return r;
}

Uptr R::getRCount() {
    return ResourceCache::get().resourceMap.size();
}

T *R::getTFromIdle() {
    faabric::util::UniqueLock lock(idleTask.getMx());
    T *t = idleTask.tryDequeue(nullptr);
    if (t)
        t->setStatus(None);
    return t;
}

void R::putT2Idle(T *t) {
    WAVM_ASSERT(t && t->getStatus() == None)
    t->setStatus(Idle);
    faabric::util::UniqueLock lock(idleTask.getMx());
    idleTask.enqueue(t);
}

void R::removeTFromBusy(T *t) {
    WAVM_ASSERT(t && t->getStatus() == Busy)
    t->setStatus(None);
    faabric::util::UniqueLock lock(busyTask.lock);
    busyTask.map.removeOrFail(t->getHash());
    t->setWorkFor(nullptr);
    t->setDirectRun(false);
}

void R::putT2Busy(T *t) {
    WAVM_ASSERT(t && t->getStatus() == None)
    t->setStatus(Busy);
    faabric::util::UniqueLock lock(busyTask.lock);
    busyTask.map.addOrFail(t->getHash(), t);
}

T *R::getIdleTFromOtherR() {
    T *t;
    ResourceCache &rc = ResourceCache::get();
    Platform::RWMutex::ShareableLock shareableLock(rc.mutex); // shareableLock是读锁，又叫共享锁
    for (auto i = rc.resourceMap.begin(); i < rc.resourceMap.end(); ++i) {
        if (i->key != this->r.getHash()) {
            t = i->value->getTFromIdle();
            if (t != nullptr)
                return t;
        }
    }
    return nullptr;
}

T *R::getIdleT(R *r) {
    T *t;
    t = r->getTFromIdle();
//    if (!t)
//        t = r->getIdleTFromOtherR();
    return t;
}

CpuResource::Impl::Impl(Uptr cpu_shares, Iptr cfs_quota_us, Iptr cfs_period_us) :
        cpuShares(cpu_shares),
        cfs_quota_us(cfs_quota_us),
        cfs_period_us(cfs_period_us) {
    hash = Hash<Uptr>()(cpu_shares, cfs_quota_us);
    hash = Hash<Uptr>()(hash, cfs_period_us);
}

MemResource::Impl::Impl(Iptr mem_soft_limit, Iptr mem_hard_limit) :
        mem_soft_limit(mem_soft_limit),
        mem_hard_limit(mem_hard_limit) {
    hash = Hash<Uptr>()(mem_soft_limit, mem_hard_limit);
}

Resource::Impl::Impl(CpuResource cpu, MemResource mem) : cpu(std::move(cpu)), mem(std::move(mem)) {
    hash = Hash<Uptr>()(cpu.getHash(), mem.getHash());
}

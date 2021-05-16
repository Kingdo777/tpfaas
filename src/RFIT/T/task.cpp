//
// Created by kingdo on 10/20/20.
//

#include <cstdio>
#include <cerrno>
#include <pthread.h>
#include <ucontext.h>
#include "RFIT/T/task.h"
#include "RFIT/I/instance.h"
#include "utils/sync.h"
#include "utils/logging.h"

/**
 * getInstance 流程：
 *  若concurrent_enable==true:
 *    1. (1) 尝试从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<1>）
 *       (2) 判断T的本地任务队列的数目是否小于容量的一半？
 *           是：                                         （判断后，本地任务队列解锁<1> 全局任务队列上锁<1>）
 *              1）从F全局的I队列中尝试拉取(cap/2)数目的任务放到自己的本地队列中  （拉取之后，全局任务队列解锁<1>本地任务队列上锁<2>; 放入本地后，本地任务队列解锁<2>）
 *              2）判断I-local是否为NULL？
 *                 是：跳转到（3）
 *                 否：直接返回I-local，函数结束
 *           否：情况能保证I-local一定不为空，直接返回I-local，函数结束
 *       (3) 再次尝试从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<3>）
 *      （4） 无论I-local，是否为NULL，都直接返回    (任务获取后，本地任务队列解锁<3>）
 *  若concurrent_enable==false || concurrent_enable==true:(下面的操作则与之前绑定的F是否为并发F无关没有关系)
 *   2. (到达2,说明1的返回结果为NULL，即对应F的全局队列已经没有任何任务可以处理)
 *      (1) 遍历当前R的所有可并发F，判断其全局任务队列是否为空 （判断之前，该全局任务队列上锁<2>）
 *          若空：寻找下一个 （该全局任务队列解锁<2>）
 *          若不空：
 *              1) 修改自己的work_for
 *              2）从该F全局的I队列中尝试拉取(cap/2)数目的任务放到自己的本地队列中 （拉取之后，该全局任务队列解锁<2>本地任务队列上锁<4>; 放入本地后，本地任务队列解锁<4>）
 *              3) 从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<5> 任务获取后，本地任务队列解锁<5>）
 *              4）I-local一定不为NULL，返回，函数结束
 *      (2) 遍历结束，函数未返回，说明当前R的所有F都不存在任务可用
 *   3. 任务偷取
 *      (1) 遍历当前R的所有为可并发F服务的busy_T_list，判断其本地队列的大小 (判断之前，该本地任务队列上锁<1>）
 *          若空：寻找下一个 （该本地任务队列解锁<1>）
 *          若不空：
 *              1) 修改自己的work_for
 *              2）从该本地I队列中尝试拉取(list_current_count/2)数目的任务放到自己的本地队列中 (拉取之后，该本地任务队列解锁<1>本地任务队列上锁<6>; 放入本地后，本地任务队列解锁<6>）
 *              3) 从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<7> 任务获取后，本地任务队列解锁<7>）
 *              4）I-local一定不为NULL，返回，函数结束
 *      (2) 遍历结束，函数未返回，说明当前R的所有F都不存在任务可用
 *   4. 尽力了，当前R已经实在无任务可以执行，放入idle_task_list
 *
 *   note:
 *      we don't get I from other R'_F'_global_I_queue or  R'_busy_task_list,
 *      because when a busy_R need an idle_T, it will steal one from other R.
 *
 *
 *
 * */
Uptr T::tryGetIFromF2T_safe(Uptr count) {
    if (workFor->getICount() < 1 || count < 1)
        return 0;
    vector<I *> iVector;
    workFor->tryGetMultiI(iVector, count);
    if (iVector.empty())
        return 0;
    addMultiI(iVector);
    return iVector.size();
}

Uptr T::tryGetIFromOtherT2T_safe(T *other_t) {
    if (other_t->getICount() <= other_t->workFor->getConcurrencyLimit() / 2)
        return 0;
    faabric::util::UniqueLock lockOtherLocal(other_t->localIMap.lock);
    Uptr pullCount = other_t->getICount() / 2;
    vector<I *> iVector;
    for (auto item :other_t->localIMap.map) {
        iVector.push_back(item.value);
        localIMap.map.remove(item.key);
        if (iVector.size() == pullCount)
            break;
    }
    addMultiI(iVector);
    return pullCount;
}

Uptr T::takeIFromILocalMap_safe(I *&i) {
    if (!localIMap.map.size()) {
        i = nullptr;
        return 0;
    }
    faabric::util::UniqueLock lock(localIMap.lock);
    auto item = localIMap.map.begin();
    i = item->value;
    localIMap.map.remove(item->key);
    return localIMap.map.size();
}

void T::addMultiI(vector<I *> &iVector) {
    faabric::util::UniqueLock lockLocal(localIMap.lock);
    for (I *item:iVector) {
        localIMap.map.add(item->getHash(), item);
    }
}

void T::attachToF_safe(F *f) {
    if (f->isConcurrency()) {
        f->scaleCap(f->getConcurrencyLimit());
        workFor = f;
    }
}

void T::leaveFromF_safe() {
    WAVM_ASSERT(workFor != nullptr)
    if (workFor->isConcurrency()) {
        workFor->scaleCap((0 - workFor->getConcurrencyLimit()));
    }
}

/**
 * 实现步骤1：
 * */
I *T::get_I_from_I_local_task_queue_and_supply_I_from_F_if_need() {
    I *i = nullptr;
    if (!workFor->isConcurrency()) {
        return i;
    }
    // 取一个I，并返回剩余任务数
    Uptr localCount = takeIFromILocalMap_safe(i);
    // 若剩余任务数小于并发限制的一半，则尝试从全局队列拉取
    Uptr halfConcurrencyLimit = workFor->getConcurrencyLimit() / 2;
    if (localCount <= halfConcurrencyLimit) {
        // 若成功拉去一定数目的I，则判断i是否为空，若是则再次从本地获取
        if (tryGetIFromF2T_safe(halfConcurrencyLimit) && !i) {
            takeIFromILocalMap_safe(i);
            WAVM_ASSERT(i != nullptr)
        }
        if (nullptr == i) {
            // 将F脱了T，因为T的全局队列已经没有任务了，所以是在缩减其任务容量
            leaveFromF_safe();
        }
        return i;
    } else {
        WAVM_ASSERT(i != nullptr)
        return i;
    }
}

/**
 * 实现步骤2 (具体实现上处于代码优化的考虑和算法并不完全一致)：
 * */
I *T::get_instance_form_R_all_F_global_queue() {
    I *i = nullptr;
    for (F *f :workFor->getR()->getFunctions()) {
        if (f->isConcurrency()) {
            if (tryGetIFromF2T_safe(f->getConcurrencyLimit() / 2)) {
                takeIFromILocalMap_safe(i);
                WAVM_ASSERT(i != nullptr)
                attachToF_safe(f);
                return i;
            }
        }
    }
    return i;
}

/**
 * 实现步骤3：
 * */
I *T::steal_instance_form_R_all_busy_T_local_queue() {
    R *r = workFor->getR();
    if (!r->getBusyTaskCount_safe())
        return nullptr;
    I *i = nullptr;
    F *f;
    T *otherT;
    faabric::util::UniqueLock lock(r->getBusyTaskLock());
    for (auto item = r->getBusyTask().begin(); item < r->getBusyTask().end(); ++item) {
        otherT = item->value;
        f = otherT->getWorkFor();
        if (f->isConcurrency() &&
            (otherT->getICount() >= otherT->workFor->getConcurrencyLimit()) &&
            (otherT->getICount() / 2) > 0) {
            if (tryGetIFromOtherT2T_safe(otherT)) {
                takeIFromILocalMap_safe(i);
                WAVM_ASSERT(i != nullptr)
                attachToF_safe(f);
                return i;
            }
        }
    }
    return i;
}

void getInstance(T *t) {
    I *i;
    R *r;
    find:
    if (t->getDealWith() != nullptr && t->getWorkFor() != nullptr && !t->getWorkFor()->isConcurrency() &&
        t->isDirectRun()) {
        t->setDirectRun(false);
        return;
    }
    i = t->get_I_from_I_local_task_queue_and_supply_I_from_F_if_need();
#ifdef STEAL_INSTANCE
    if (nullptr == i) {
        i = t->get_instance_form_R_all_F_global_queue();
    }
    if (nullptr == i) {
        i = t->steal_instance_form_R_all_busy_T_local_queue();
    }
#endif
    if (nullptr != i) {
        t->setWorkFor(i->getF());
        t->setDealWith(i);
        return;
    }
    r = t->getWorkFor()->getR();
    r->removeTFromBusy(t);
    r->putT2Idle(t);
    // 这个地方那个存在一个FUTEX的BUG,当把一个T放入idle之后,这个T马上就会被感知的,并被从idle中拿出,并尝试唤醒,由于此时还没有进行sleep,
    // 这将导致,唤醒操作没有意义,且这个T在执行睡眠后将无法在被唤醒
    // 这个Bug暂时无解,因为,要想保证上述情况只能加锁,但是睡眠之后无法解锁,因此锁机制没法用,目前只能使用信号量来解决
    thread_sleep(t);
    goto find;
}

//#define PRINT_TGID
static void *task_done(void *arg) {
    T *t = (T *) arg;
    getLogger()->debug("Create A new T : <{}>", t->toString());
    getcontext(&t->getTaskContext());
    if (!t->isDirectRun() && t->getDealWith()) {
        if (t->getWorkFor()->isConcurrency()) {
            t->getWorkFor()->scaleCap(1);
        }
        //删除instance
        t->getDealWith()->release_instance_space();
    }
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    getInstance(t);
    //找到的instance被写到了task的deal_with中
    I *i = t->getDealWith();
    i->getUcontext().uc_link = &t->getTaskContext();
#ifdef FCGI_ENABLE
    makecontext(&i->getUcontext(), (void (*)()) i->getF()->getEntryAddr(), 1,
                i->getRequest());
#endif
#ifdef PRINT_TGID
    makecontext(&i->getUcontext(), (void (*)()) i->getF()->getEntryAddr(), 1,
                t->getTgid());
#else
    makecontext(&i->getUcontext(), (void (*)()) i->getF()->getEntryAddr(), 1,
                i->getRequest());
#endif
    setcontext(&i->getUcontext());
    return nullptr;
}

//这里返回的T一定是纯净的，不在任何链表中的(idle\busy\work_for_F)
//如果返回的是NULL说明是新创建的。直接为I服务不需要唤醒
void T::get_T_for_I(I *i, F *f) {
    T *t = R::getIdleT(f->getR());
    if (t) {
        if (!f->isConcurrency()) {
            t->dealWith = i;
            t->directRun = true;
        }
        t->workFor = f;
        f->getR()->putT2Busy(t);
        thread_wake_up_one(t);
    } else
        new T(f, i);
}

T::T(F *f, I *i) {
    sem_init(&semWord, 0, 0);
    status = None;
    workFor = f;
    hash = Hash<Uptr>()(i->getHash());
    if (!f->isConcurrency()) {
        dealWith = i;
        directRun = true;
    }
    f->getR()->putT2Busy(this);
    bind_os_thread();
}

bool T::bind_os_thread() {
    Iptr try_c = TRY_COUNT;
    Iptr ret;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    try_:
    ret = pthread_create(&tgid, &attr, task_done, this);
    if (ret != 0) {
        switch (ret) {
            case EAGAIN:
                fprintf(stderr,
                        "pthread_create wrong:A system-imposed limit on the number of threads was encountered or "
                        "Insufficient resources to create another thread." "\n");
                break;
            case EINVAL:
                fprintf(stderr, "pthread_create wrong:Invalid settings in attr." "\n");
                break;
            case EPERM:
                fprintf(stderr,
                        "pthread_create wrong:No permission to set the scheduling policy and parameters specified in attr." "\n");
                break;
            default:
                fprintf(stderr, "pthread_create wrong:unknown wrong." "\n");
                break;
        }
        printf("clone wrong,try %ld\n", TRY_COUNT - try_c + 1);
        if (try_c-- > 0)
            goto try_;
        printf("clone fault,errno:%d：\n", errno);
        return false;
    }
    return true;
}
//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include <semaphore.h>
#include <WAVM/Inline/HashMap.h>
#include <csignal>
#include <mutex>
#include <string>

using namespace WAVM;
using namespace std;

#define TRY_COUNT 3
#define T_STACK_SIZE (8*1024*1024) //8MB
#define TASK_MAX_COUNT 100

class F;

class I;

//T的三种存在状态
enum T_status {
    Idle,
    Busy,
    None,
};

class T {
public:
    T_status getStatus() { return status; };

    void setStatus(T_status s) { status = s; };


    static void get_T_for_I(I *i, F *f);

    [[nodiscard]] F *getWorkFor() const {
        return workFor;
    }

    void setWorkFor(F *f) {
        T::workFor = f;
    }

    [[nodiscard]] I *getDealWith() const {
        return dealWith;
    }

    void setDealWith(I *i) {
        T::dealWith = i;
    }

    [[nodiscard]] bool isDirectRun() const {
        return directRun;
    }

    void setDirectRun(bool b) {
        T::directRun = b;
    }

    [[nodiscard]]  ucontext_t &getTaskContext() { return taskContext; };

    pthread_t &getTgid() {
        return tgid;
    }

    [[nodiscard]] Uptr getHash() const {
        return hash;
    }

    [[nodiscard]] Uptr getICount() const {
        return localIMap.map.size();
    }

    I *get_I_from_I_local_task_queue_and_supply_I_from_F_if_need();

    I *get_instance_form_R_all_F_global_queue();

    I *steal_instance_form_R_all_busy_T_local_queue();

    [[nodiscard]]  sem_t &getSemWord() {
        return semWord;
    }

    [[nodiscard]] string toString() const {
        return "T::tgid-" + to_string(tgid);
    }

private:
    Uptr hash;
    //pid
    pthread_t tgid{};

    sem_t semWord{};

    //当前正在为那个F工作
    F *workFor = nullptr;

    //正在处理Instance
    I *dealWith = nullptr;

    //针对T正在处理不能并发的函数，此时T是直接被指定一个I的，而不需要自己寻找，此标志位帮助get_instance区分
    bool directRun = false;

    ucontext_t taskContext{};

    struct LocalMap {
        std::mutex lock;
        HashMap<Uptr, I *> map;
    };
    LocalMap localIMap;

    T_status status;

    bool bind_os_thread();

    Uptr tryGetIFromF2T_safe(Uptr count);

    Uptr takeIFromILocalMap_safe(I *&i);

    Uptr tryGetIFromOtherT2T_safe(T *other_t);

    void addMultiI(vector<I *> &iVector);

    void attachToF_safe(F *f);

    void leaveFromF_safe();

    explicit T(F *f, I *i = nullptr);

};

#endif //TPFAAS_TASK_H

//
// Created by kingdo on 2020/10/23.
//
#include <RFIT/F/function.h>
#include <malloc.h>
#include <utils/logging.h>
#include "RFIT/I/instance.h"
#include "RFIT/T/task.h"


I::I(F *f, FCGX_Request *request) : f(f), request(request), timer(startTimer()) {
    getcontext(&ucontext);
    ucontext.uc_stack.ss_sp = malloc(T_STACK_SIZE);
    ucontext.uc_stack.ss_size = T_STACK_SIZE;
    hash = Hash<Uptr>()((Uptr) request);
    getLogger()->debug("Make A new I : <" + this->toString() + ">");
}

I::~I() {
    free(ucontext.uc_stack.ss_sp);
    getLogger()->trace(string() + "I LifeTime: <{:.3f}ms> " + "<" + this->toString() + ">",
                       getTimeDiffMillis(timer));
}

void I::make_request(F *f, FCGX_Request *req) {
    I *i = new I(f, req);
    if (i->addI2F())
        //此时,如果F是可并发的,也就意味着,i已经被放入了全局F队列中,I随时可能被拿去执行,然后被释放,那么i就不可用了
        T::get_T_for_I(i, f);
}

//返回值表示，是否需要将唤醒或者启动一个新的T来执行请求
bool I::addI2F() {
    // 如果是不支持并发的，那么一定I不需要进入全局队列，而是直接唤醒一个T来处理
    if (!f->isConcurrency())
        return true;
    // 如果是支持并发的，那么先将其加入到全局队列
    // 如果当前有可用容量，那么直接容量减1;否则唤醒一个T，并扩容C-1
    f->addI2F(this);
    return !(f->tryScaleDownCap() || !(f->scaleCap(f->getConcurrencyLimit() - 1), true));
}

void I::release_instance_space() {
    delete this;
}
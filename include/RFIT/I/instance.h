//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_INSTANCE_H
#define TPFAAS_INSTANCE_H

#include "RFIT/F/function.h"
#include "utils/timing.h"
#include <ucontext.h>
#include <fcgiapp.h>


class I {
public:
    static void make_request(F *f, FCGX_Request *req);

    bool addI2F();

    void release_instance_space();

    [[nodiscard]] Uptr getHash() const { return hash; };

    ucontext_t &getUcontext() {
        return ucontext;
    }

    [[nodiscard]] F *getF() const {
        return f;
    }

    [[nodiscard]] FCGX_Request *getRequest() const {
        return request;
    }

    string toString() {
        return "I::F-" + f->getName() + "::Hash-" + to_string(hash);
    }

    ~I();

private:
    Uptr hash;
    F *f;
    FCGX_Request *request;
    ucontext_t ucontext{};
    TimePoint timer;

    I(F *f, FCGX_Request *request);

};

#endif //TPFAAS_INSTANCE_H

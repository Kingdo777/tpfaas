#pragma once

#include <utils/config.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include "TpfaasEndpointHandler.h"

class Endpoint {
public:
    Endpoint() = default;

    Endpoint(int port, int threadCount);

    void start();

    // 创建虚函数，虚函数将保证：当通过基类的指针或者引用调用该成员函数时，将根据指针指向的对象类型确定调用的函数，而非指针的类型
    // =0 表示这是一个纯虚函数，也就是只能被继承
    // 见 https://blog.csdn.net/xbb123456rt/article/details/81986691
    // virtual std::shared_ptr<Pistache::Http::Handler> getHandler() = 0;
    std::shared_ptr<Pistache::Http::Handler> getHandler() {
        return Pistache::Http::make_handler<TpfaasEndpointHandler>();
    }

private:
    int port = getSystemConfig().endpointPort;               // 默认8080
    int threadCount = getSystemConfig().endpointNumThreads;  // 默认1
};

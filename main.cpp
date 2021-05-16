//
// Created by kingdo on 10/17/20.
//
#ifdef FCGI_ENABLE

#include <test/test.h>
#include "RFIT/I/instance.h"
#include <fcgiapp.h>
#include <malloc.h>
#include <RFIT/R/resource.h>

#define Q_COUNT 100000

int main() {
    int rc;
    FCGX_Request *request;
    //输出重定向
    freopen("stdout-log", "w", stdout);
    freopen("stderr-log", "w", stderr);
    //初始化FCGI
    FCGX_Init();
    F::registerF("taskA", taskF, Resource(), 100);
    F *f_A = F::getF("taskA");
    while (true) {
        request = (FCGX_Request *) malloc(sizeof(FCGX_Request));
        FCGX_InitRequest(request, 0, 0);
        rc = FCGX_Accept_r(request);
        printf("receive R\n");
        if (rc < 0) {
            printf("rc < 0\n");
            FCGX_Finish_r(request);
            free(request);
            break;
        }
#ifdef ONLY_POST
        if (strcmp("POST",FCGX_GetParam("REQUEST_METHOD",request->envp))!=0){
            printf("REQUEST_METHOD is not POST\n");
            FCGX_Finish_r(request);
            free(request);
            break;
        }
#endif
        printf("receive R\n");
        I::make_request(f_A, request);
    }
}

#else

#include <test/test.h>
#include "RFIT/I/instance.h"
#include <fcgiapp.h>
#include <zconf.h>
#include <RFIT/R/resource.h>

#define Q_COUNT 500000

int main() {
    F *f_B = F::getF("taskB");
    F::registerF("taskB", taskT, Resource(), 1);
    f_B = F::getF("taskB");
    for (Uptr i = 1; i <= Q_COUNT; ++i) {
        I::make_request(f_B, reinterpret_cast<FCGX_Request *>(i));
    }
    sleep(10);
    WAVM_ASSERT(f_B->getR()->getBusyTask().size()==0)
    printf("SUCCESS!!\n");
    sleep(1000000);
}

#endif
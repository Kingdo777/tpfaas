//
// Created by kingdo on 10/17/20.
//

#include <stdio.h>
#include <include/test.h>
#include "include/lock.h"
#include "include/instance.h"
#include <fcgiapp.h>
void debug_trace_init();

#define Q_COUNT 50000

int main() {
    int rc;
    FCGX_Request *request;
    //输出重定向
    freopen("stdout-log","w",stdout);
    freopen("stderr-log","w",stderr);
    //配置调试的堆栈输出
    debug_trace_init();
    //初始化锁
    INIT_LOCK()
    //初始化FCGI
    FCGX_Init();
    //临时代码
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 100);
    F *f_A = get_func("taskA");
    while (1) {
        request=(FCGX_Request *)malloc(sizeof(FCGX_Request));
        FCGX_InitRequest(request, 0, 0);
        rc = FCGX_Accept_r(request);
        if (rc < 0){
            printf("rc < 0\n");
            free(request);
            break;
        }
        make_request(f_A, request);
    }
}

//int main1() {
//    INIT_LOCK()
//    resource res = DEFAULT_RESOURCE;
//    func_register((void *(*)(void *)) taskF, "taskA", res, 100);
//    F *f_A = get_func("taskA");
//
//    struct timeval start[Q_COUNT], end[Q_COUNT];
//    for (int i = 1; i <= Q_COUNT; ++i) {
//        gettimeofday(&start[i - 1], NULL);  /*测试起始时间*/
//        make_request(f_A, i);
////        usleep(10 * MS_US);
//        gettimeofday(&end[i - 1], NULL);   /*测试终止时间*/
//    }
//
//
//    sleep(5);
//
//    long time_diff[Q_COUNT];
//    long mean = 0;
//    for (int i = 0; i < Q_COUNT; ++i) {
//        time_diff[i] = (end[i].tv_sec - start[i].tv_sec) * (S_NS / US_NS) + end[i].tv_usec - start[i].tv_usec;
//        mean += time_diff[i];
//    }
//    mean /= Q_COUNT;
//    qsort(time_diff, Q_COUNT, sizeof(long), cmp_long);
//    printf("Min:\t\t%ldus\nMax:\t\t%ldus\nMiddle:\t\t%ldus\nMean:\t\t%ldus\n", time_diff[0], time_diff[Q_COUNT - 1],
//           time_diff[Q_COUNT / 2], mean);
//
//
//    printf("done\n");
//    for (int i = 0; i < 3; ++i) {
//        print_task_info();
//        sleep(10);
//    }
//    sleep(1000000);
//    return 0;
//}

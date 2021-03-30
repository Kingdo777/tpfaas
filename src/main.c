//
// Created by kingdo on 10/17/20.
//

#include <stdio.h>
#include <include/test.h>
#include "include/lock.h"
#include "include/instance.h"
#include <fcgiapp.h>
#include <zconf.h>
#include <task.h>
#include <malloc.h>

void debug_trace_init();

#define Q_COUNT 500000

//int main() {
//    int rc;
//    FCGX_Request *request;
//    //输出重定向
//    freopen("stdout-log","w",stdout);
//    freopen("stderr-log","w",stderr);
//    //配置调试的堆栈输出
//    debug_trace_init();
//    //初始化锁
//    INIT_LOCK()
//    //初始化FCGI
//    FCGX_Init();
//    //临时代码
//
//
//
//
//    resource res = DEFAULT_RESOURCE;
//    func_register(taskF, "taskA", res, 100);
//    F *f_A = get_func("taskA");
//    while (1) {
//        request=(FCGX_Request *)malloc(sizeof(FCGX_Request));
//        FCGX_InitRequest(request, 0, 0);
//        rc = FCGX_Accept_r(request);
//        if (rc < 0){
//            printf("rc < 0\n");
//            FCGX_Finish_r(request);
//            free(request);
//            break;
//        }
//
////        if (strcmp("POST",FCGX_GetParam("REQUEST_METHOD",request->envp))!=0){
////            printf("REQUEST_METHOD is not POST\n");
////            FCGX_Finish_r(request);
////            free(request);
////            break;
////        }
//        make_request(f_A, request);
//    }
//}

pthread_mutex_t count_mutex;
uint malloc_count = 0;
uint free_count = 0;
uint free_count_stack = 0;
uint free_count_I = 0;

void *task_done(void *arg);

//void *fun(void *arg) {
//    struct ucontext_t c;
//    int i = 0;
//    getcontext(&c);
//    if (i++ == 10)
//        goto end;
//    setcontext(&c);
//    end:;
//}
//
//#define WWWWWWWWW 3000

int main() {
//    mallopt(M_MMAP_THRESHOLD,0);
//    pthread_t pt[WWWWWWWWW];
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//    for (int i = 0; i < WWWWWWWWW; ++i) {
//        pthread_create(&pt[i], &attr, fun, NULL);
//    }
//    sleep(1000);
//    return 0;
    pthread_mutex_init(&count_mutex, NULL);
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskT, "taskB", res, 100);
    F *f_B = get_func("taskB");
    for (int i = 1; i <= Q_COUNT; ++i) {
        make_request(f_B, i);
    }

    sleep(100);
    /**
     * 删除R、F、T、释放所有申请的线程
     * */

    sleep(1);
    printf("\nMalloc %d\nFree %d\nFree_stack %d\nFree_I %d\n", malloc_count, free_count, free_count_stack,
           free_count_I);
    T **t = (T **) malloc(malloc_count * sizeof(T *));
    T *t1;
    list_for_each_entry(t1, &f_B->r->task_idle_head, task_idle_list) {
        t[free_count++] = t1;
    }
    list_for_each_entry(t1, &f_B->r->task_busy_head, task_busy_list) {
        t[free_count++] = t1;
    }
    for (int i = 0; i < free_count; ++i) {
        pthread_cancel(t[i]->tgid);
        pthread_join(t[i]->tgid, NULL);
        free(t[i]);
    }
    free(f_B->r);
    free(f_B);
    free(t);
    printf("\nMalloc %d\nFree %d\nFree_stack %d\nFree_I %d\n", malloc_count, free_count, free_count_stack,
           free_count_I);
    sleep(100);
    malloc_trim(0);
    printf("Release All Memory\n");

    sleep(2000);
}

//int main() {
//    INIT_LOCK()
//    resource res = DEFAULT_RESOURCE;
//    func_register(taskT, "taskB", res, 100);
//    F *f_B = get_func("taskB");

//    struct timeval start[Q_COUNT], end[Q_COUNT];
//    for (int i = 1; i <= Q_COUNT; ++i) {
//        gettimeofday(&start[i - 1], NULL);  /*测试起始时间*/
//        make_request(f_B, i);
//        usleep(10 * MS_US);
//        gettimeofday(&end[i - 1], NULL);   /*测试终止时间*/
//    }


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


//    printf("done\n");
//    for (int i = 0; i < 3; ++i) {
//        print_task_info();
//        sleep(10);
//    }
//    sleep(1000000);
//    return 0;
//}

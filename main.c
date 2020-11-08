//
// Created by kingdo on 10/17/20.
//

#include <stdio.h>
#include <unistd.h>
#include <test/test.h>
#include <sys/time.h>
#include <tool/sort.h>
#include "tool/print.h"
#include "sync/lock.h"
#include "instance/instance.h"

#define Q_COUNT 1000

int main() {
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 10);
    F *f_A = get_func("taskA");

    struct timeval start[Q_COUNT], end[Q_COUNT];
    for (int i = 1; i <= Q_COUNT; ++i) {
        gettimeofday(&start[i - 1], NULL);  /*测试起始时间*/
        make_request(f_A, (void *) i);
//        usleep(10 * MS_US);
        gettimeofday(&end[i - 1], NULL);   /*测试终止时间*/
    }

    long time_diff[Q_COUNT];
    long mean = 0;
    for (int i = 0; i < Q_COUNT; ++i) {
        time_diff[i] = (end[i].tv_sec - start[i].tv_sec) * (S_NS / US_NS) + end[i].tv_usec - start[i].tv_usec;
        mean += time_diff[i];
    }
    mean /= Q_COUNT;
    qsort(time_diff, Q_COUNT, sizeof(long), cmp_long);
    printf("Min:\t\t%ld\nMax:\t\t%ld\nMiddle:\t\t%ld\nMean:\t\t%ld\n", time_diff[0], time_diff[Q_COUNT - 1],
           time_diff[Q_COUNT / 2], mean);


    printf("done\n");
    for (int i = 0; i < 3; ++i) {
        print_task_info();
        sleep(10);
    }
    sleep(1000000);
    return 0;
}

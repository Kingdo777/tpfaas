//
// Created by kingdo on 10/18/20.
//

#ifndef TPFAAS_RESOURCE_H
#define TPFAAS_RESOURCE_H

#include "tool/list_head.h"
#include "pthread.h"

#define CPU_DEFAULT_SHARES 1024
#define CPU_DEFAULT_CFS_QUOTA_US -1
#define CPU_DEFAULT_CFS_PERIOD_US 100000

#define DEFAULT_CPU_RESOURCE {CPU_DEFAULT_SHARES,CPU_DEFAULT_CFS_QUOTA_US,CPU_DEFAULT_CFS_PERIOD_US}
#define DEFAULT_MEM_RESOURCE {}
#define DEFAULT_RESOURCE {DEFAULT_CPU_RESOURCE,DEFAULT_MEM_RESOURCE}

typedef struct {
    //function的资源限制
    //cpu limit
    int cpu_shares;
    int cfs_quota_us;
    int cfs_period_us;
} cpu_resource;
#define CPU_RESOURCE_EQUAL(cpu_src, cpu_dst) (cpu_src.cfs_period_us == cpu_dst.cfs_period_us &&\
                                    cpu_src.cfs_quota_us == cpu_dst.cfs_quota_us &&\
                                    cpu_src.cpu_shares == cpu_dst.cpu_shares)
typedef struct {
    //mem limit
    //TODO 因为mem的部分比较复杂，因此暂时不写
} mem_resource;
#define MEM_RESOURCE_EQUAL(mem_src, cpu_dst) (true)

typedef struct {
    cpu_resource cpu;
    mem_resource mem;
} resource;
#define RESOURCE_EQUAL(src, dst) (CPU_RESOURCE_EQUAL(src.cpu,dst.cpu)&&MEM_RESOURCE_EQUAL(src.mem,dst.mem))


typedef struct {
    resource res;

    pthread_mutex_t task_idle_lock;
    int task_idle_count;
    //链接在此R上的所有的空闲T
    list_head task_idle_head;

    pthread_mutex_t task_busy_lock;
    int task_busy_count;
    //链接在此R上的所有的工作T
    list_head task_busy_head;

    //用于链接每个list
    list_head res_list;
    //链接在此R上的所有F
    list_head resource_func_head;
} R;

#define INIT_R_LIST_HEAD(r) do{  \
    INIT_LIST_HEAD(&r->res_list);\
    INIT_LIST_HEAD(&r->resource_func_head);\
    INIT_LIST_HEAD(&r->task_idle_head);\
    INIT_LIST_HEAD(&r->task_busy_head);\
}while(0);

R *get_res(resource res);

#endif //TPFAAS_RESOURCE_H

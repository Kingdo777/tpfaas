//
// Created by kingdo on 10/18/20.
//

#ifndef TPFAAS_RESOURCE_H
#define TPFAAS_RESOURCE_H

#include "tool/list_head.h"

#define CPU_DEFAULT_SHARES 1024
#define CPU_DEFAULT_CFS_QUOTA_US -1
#define CPU_DEFAULT_CFS_PERIOD_US 100000

#define DEFAULT_CPU_RESOURCE {CPU_DEFAULT_SHARES,CPU_DEFAULT_CFS_QUOTA_US,CPU_DEFAULT_CFS_PERIOD_US}
#define DEFAULT_MEM_RESOURCE {}
#define DEFAULT_RESOURCE {DEFAULT_CPU_RESOURCE,DEFAULT_MEM_RESOURCE}

typedef struct cpu_resource {
    //function的资源限制
    //cpu limit
    int cpu_shares;
    int cfs_quota_us;
    int cfs_period_us;
} cpu_resource;

typedef struct mem_resource {
    //mem limit
    //TODO 因为mem的部分比较复杂，因此暂时不写
} mem_resource;

typedef struct resource {
    cpu_resource cpu;
    mem_resource mem;
} resource;

typedef struct R {
    resource res;

    //用于链接每个list
    list_head res_list;
    //链接在此R上的所有F
    list_head function_head;
    //链接在此R上的所有的空闲T
    list_head task_idle_head;
    //链接在此R上的所有的工作T
    list_head task_busy_head;
} R;

#endif //TPFAAS_RESOURCE_H

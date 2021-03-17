//
// Created by kingdo on 2020/11/6.
//

#ifndef TPFAAS_SORT_H
#define TPFAAS_SORT_H

#define CMP_ANY(a, b) ((a)>(b))?1:(((a)<(b))?-1:0)
#define CMP_TYPE(a, b, type) CMP_ANY((*(type*)(a)),(*(type*)(b)))

static inline int cmp_long(const void *a, const void *b) {
    return CMP_TYPE(a, b, long);
}

static inline int cmp_int(const void *a, const void *b) {
    return CMP_TYPE(a, b, int);
}

static inline int cmp_timeval(const void *a, const void *b) {
    long a_ = (*(struct timeval *) a).tv_usec;
    long b_ = (*(struct timeval *) b).tv_usec;
    return CMP_ANY(a_, b_);
}


#endif //TPFAAS_SORT_H

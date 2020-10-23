//
// Created by kingdo on 10/19/20.
//

#ifndef TPFAAS_TLS_H
#define TPFAAS_TLS_H

#define TLS(t) T *t;\
    get_tls(&t)

int arch_prctl(int code, unsigned long *addr);

void get_tls(void *tls_addr);

/**
 *
 * */
void set_tls(void *tls_addr);

void get_tls_gs(void *tls_addr);

void set_tls_gs(void *tls_addr);

#endif //TPFAAS_TLS_H

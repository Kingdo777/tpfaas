//
// Created by kingdo on 10/17/20.
//
#include <stdio.h>
#include <pthread.h>
#include <zconf.h>

void * task(void *arg){
    sleep(10);
    printf("I am Task");
}

int main(){
    printf("hello world\n");
    pthread_t task_id ;
    pthread_create(&task_id,NULL,task,NULL);
    pthread_join(task_id,NULL);
    return 0;
}

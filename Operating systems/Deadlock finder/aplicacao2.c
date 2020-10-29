#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

sem_t s1, s2;

void *p1(void *args){
    while(1){
        printf("Processo 1\n"); fflush(stdout);
        if(sem_wait(&s1)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&s2)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&s1);
        sem_post(&s2);
    }
}

void *p2(void *args){
    while(1){
        printf("Processo 2\n"); fflush(stdout);
        if(sem_wait(&s2)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&s1)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&s1);
        sem_post(&s2);
    }
}

int main(){
    pthread_t t1, t2;
    sem_init(&s1, 0, 1);
    sem_init(&s2, 0, 1);

    pthread_create(&t1, NULL, p1, NULL);
    pthread_create(&t2, NULL, p2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

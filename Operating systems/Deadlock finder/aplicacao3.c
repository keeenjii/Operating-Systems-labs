#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

sem_t R, S, T, U, V, W;

void *pA(void *args){
    while(1){
        printf("Processo A\n"); fflush(stdout);
        if(sem_wait(&R)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&S)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&R);
        sem_post(&S);
    }
}

void *pB(void *args){
    while(1){
        printf("Processo B\n"); fflush(stdout);
        if(sem_wait(&T)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&T);
    }
}

void *pC(void *args){
    while(1){
        printf("Processo C\n"); fflush(stdout);
        if(sem_wait(&S)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&S);
    }
}

void *pD(void *args){
    while(1){
        printf("Processo D\n"); fflush(stdout);
        if(sem_wait(&U)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&T)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&S)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&U);
        sem_post(&T);
        sem_post(&S);
    }
}

void *pE(void *args){
    while(1){
        printf("Processo E\n"); fflush(stdout);
        if(sem_wait(&T)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&V)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&T);
        sem_post(&V);
    }
}

void *pF(void *args){
    while(1){
        printf("Processo F\n"); fflush(stdout);
        if(sem_wait(&W)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&S)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&W);
        sem_post(&S);
    }
}

void *pG(void *args){
    while(1){
        printf("Processo G\n"); fflush(stdout);
        if(sem_wait(&V)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        if(sem_wait(&U)){
            printf("\n%s\n", strerror(errno));
            exit(0);
        }
        sem_post(&V);
        sem_post(&U);
    }
}


int main(){
    pthread_t tA, tB, tC, tD, tE, tF, tG;
    sem_init(&R, 0, 1);
    sem_init(&S, 0, 1);
    sem_init(&T, 0, 1);
    sem_init(&U, 0, 1);
    sem_init(&V, 0, 1);
    sem_init(&W, 0, 1);

    pthread_create(&tA, NULL, pA, NULL);
    pthread_create(&tB, NULL, pB, NULL);
    pthread_create(&tC, NULL, pC, NULL);
    pthread_create(&tD, NULL, pD, NULL);
    pthread_create(&tE, NULL, pE, NULL);
    pthread_create(&tF, NULL, pF, NULL);
    pthread_create(&tG, NULL, pG, NULL);

    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
    pthread_join(tC, NULL);
    pthread_join(tD, NULL);
    pthread_join(tE, NULL);
    pthread_join(tF, NULL);
    pthread_join(tG, NULL);
}

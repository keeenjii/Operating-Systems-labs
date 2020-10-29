#define _GNU_SOURCE
#define DEADLOCK 200

#include "graph.h"
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>

Graph *G;
int call = 0;
pthread_mutex_t op;

int (*_sem_init)(sem_t *, int, unsigned int) = NULL;
int sem_init(sem_t *sem, int pshared, unsigned int value){
	int r;

	if(!_sem_init){
		_sem_init = dlsym(RTLD_NEXT, "sem_init");
	}
	if(call == 0){
		pthread_mutex_init(&op, NULL);
		G = createGraph();
		call += 1;
	}else{
		insertVertex(G, (size_t)sem, SEM);
	}

	r = _sem_init(sem, pshared, value);
	return(r);
}

int (*_sem_wait)(sem_t *) = NULL;
int sem_wait(sem_t *sem){
	int r;

	if(!_sem_wait){
		_sem_wait = dlsym(RTLD_NEXT, "sem_wait");
	}

	pthread_mutex_lock(&op);
	createEdge(G, pthread_self(), PRO, (size_t)sem, SEM);
	pthread_mutex_unlock(&op);

	pthread_mutex_lock(&op);
	if(isCyclical(G)){
		errno = DEADLOCK;
		return -1;
	}
	pthread_mutex_unlock(&op);

	r = _sem_wait(sem);

	pthread_mutex_lock(&op);
	deleteEdge(G, pthread_self(), (size_t)sem);
	createEdge(G, (size_t)sem, SEM, pthread_self(), PRO);
	pthread_mutex_unlock(&op);

	return(r);
}

int (*_sem_post)(sem_t *) = NULL;
int sem_post(sem_t *sem){
	int r;

	if(!_sem_post){
		_sem_post = dlsym(RTLD_NEXT, "sem_post");
	}

	r = _sem_post(sem);
	pthread_mutex_lock(&op);
	deleteEdge(G, (size_t)sem, pthread_self());
	pthread_mutex_unlock(&op);

	return(r);
}

char* (*_strerror)(int) = NULL;
char* strerror(int errnum){
	char* r;

	if(!_strerror){
		_strerror = dlsym(RTLD_NEXT, "strerror");
	}

	if(errnum != DEADLOCK){
		r = _strerror(errnum);
	}else{
		r = "Deadlock found";
	}

	return(r);
}
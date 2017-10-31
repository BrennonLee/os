#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <stdio.h>
#include <stdlib.h>	
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h> 	
#include <pthread.h>
#include "util.c"

#define ARRAYSIZE 15
pthread_cond_t writer, reader;

struct requestStruct {
	pthread_mutex_t servicedFileLock;
	pthread_mutex_t servicedCountLock;
	pthread_mutex_t inputFileLock;
	pthread_mutex_t sharedArrayLock;
	
	char** inputFiles;
	char** sharedArray;
	char* servicedFile;
	int totalServiced;
	int totalIF;
	int* sharedArrayCounter;
	int* ALIVE;
};

struct resolverStruct {
	pthread_mutex_t sharedArrayLock;
	pthread_mutex_t resultsFileLock;
	char** sharedArray;
	char* resultFile;
	int* sharedArrayCounter;
	int* ALIVE;
};


void* request(struct requestStruct* rStruct);
void* resolver(struct resolverStruct* resStruct);

#endif
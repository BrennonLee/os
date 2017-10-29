#include <stdio.h>
#include <stdlib.h>	
#include <sys/types.h>
#include <sys/syscall.h>	
#include <pthread.h>

struct requestStruct {
	pthread_mutex_t servicedFileLock;
	pthread_mutex_t servicedCountLock;
	pthread_mutex_t inputFileLock;
	pthread_mutex_t sharedArrayLock;
	char** inputFiles;
	FILE* servicedFile;
	FILE* currentIF;
	int totalServiced;
	int totalIF;
	
};
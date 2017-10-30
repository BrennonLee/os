#include "multi-lookup.h"
	/*
		Written by Brennon Lee
	*/

	//to compile program run gcc -o multi-lookup multi-lookup.c multi-lookup.h -lpthread

	//arg 2 - # of requester threads --> argv[1]
	//arg 3 - # of resolver threads --> argv[2]
	//arg 4 - results.txt	
	//arg 5 - serviced.txt
	//Then up to 10 input files.

void* request(struct requestStruct* rStruct) {
	pid_t threadID = syscall(__NR_gettid);
	char domainName[100];
	int localServiced = 0;
	FILE* currentFile;
	while(rStruct->totalServiced < rStruct->totalIF){
		pthread_mutex_lock(&rStruct->inputFileLock);
		if ((rStruct->totalServiced + 1) > rStruct->totalIF){
			break;
		}
		currentFile = fopen(rStruct->inputFiles[rStruct->totalServiced], "r+");
		if(rStruct == NULL){
			fprintf(stderr, "Can't open input file!\n");	
		}
		printf("thread %u is inside the loop with file: %s\n", threadID, rStruct->inputFiles[rStruct->totalServiced]);
		rStruct->totalServiced += 1;		
		pthread_mutex_unlock(&rStruct->inputFileLock);

		while(fscanf(currentFile, "%s", domainName) == 1){
			printf("domainName: %s\n", domainName);
		}
		fclose(currentFile);

		pthread_mutex_lock(&rStruct->inputFileLock);
		localServiced++;	
		pthread_mutex_unlock(&rStruct->inputFileLock);
	}
	pthread_mutex_lock(&rStruct->servicedFileLock);	
	rStruct->servicedFile = fopen("serviced.txt","a");
	if (rStruct->servicedFile == NULL) {
  		fprintf(stderr, "Can't open serviced file!\n");
  		exit(1);
	}
	fprintf(rStruct->servicedFile, "Thread %u serviced %d file(s)\n", threadID, localServiced);
	
	fclose(rStruct->servicedFile);
	pthread_mutex_unlock(&rStruct->servicedFileLock);

	printf("Thread %u just outside loop\n", threadID);
	return NULL;	
}






int main (int argc, char *argv[]) {	

	if (argc > 15 || argc < 6) {
		printf("Invalid arguement entry. Format is: \n");
		printf("./multi-lookup <requester threads> <resolver threads> <results.txt> <serviced.txt> <..inputfiles..>\n");
		return 0;	
	}

	
	char *inputFiles[argc - 5]; //array for input files
	int threadNum = atoi(argv[1]); //convert read in string to int
	int ifiles = argc - 5; // remaining input files
	pthread_t rthread[threadNum]; // requestor threads entered

	//loop through inputfiles and add them to inputFiles array
	for(int i=0; i < (argc - 5); i++){
		inputFiles[i] = argv[5+i];
	}

	struct requestStruct Requestor;
	struct requestStruct* RequestPtr;

	Requestor.inputFiles = inputFiles; //Requestor struct set to inputFiles array
	Requestor.totalIF = argc - 5;	//Total number of input files
	Requestor.totalServiced = 0;	//total inputFiles serviced initialized
	RequestPtr = &Requestor;	//Requestor struct pointer set to point to our Requestor struct

	pthread_mutex_init(&Requestor.servicedFileLock, NULL);
	pthread_mutex_init(&Requestor.servicedCountLock, NULL);
	pthread_mutex_init(&Requestor.inputFileLock, NULL);
	pthread_mutex_init(&Requestor.sharedArrayLock, NULL);

	for (int i=0; i < threadNum; i++){
		if(pthread_create(&rthread[i], NULL, request, RequestPtr)){
			fprintf(stderr,"Error creating thread\n");
			return 1;
		}
	}

	for(int i = 0; i < threadNum; i++){
		if (pthread_join(rthread[i],NULL)){
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}


	return 0;
}
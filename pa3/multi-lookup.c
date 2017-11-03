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
	char domainName[1025];
	int localServiced = 0;
	FILE* currentFile;
	FILE* servicedFile;
	while(rStruct->totalServiced < rStruct->totalIF){
		pthread_mutex_lock(&rStruct->inputFileLock);
		if ((rStruct->totalServiced + 1) > rStruct->totalIF){
			printf("Req thread is breaking\n");
			pthread_mutex_unlock(&rStruct->inputFileLock);
			break;
		}
		currentFile = fopen(rStruct->inputFiles[rStruct->totalServiced], "r+");
		if(currentFile == NULL){
			fprintf(stderr, "Can't open input file!\n");
		}
		printf("thread %u is inside the loop with file: %s\n", threadID, rStruct->inputFiles[rStruct->totalServiced]);
		rStruct->totalServiced += 1;
		pthread_mutex_unlock(&rStruct->inputFileLock);

		while(!feof(currentFile)){
			pthread_mutex_lock(&rStruct->sharedArrayLock);
			printf("sharedArrayCounter(IFSTATEMENT): %d\n", *rStruct->sharedArrayCounter);
			if (*rStruct->sharedArrayCounter <= 14){
				fscanf(currentFile, "%s", domainName);
				rStruct->sharedArray[*rStruct->sharedArrayCounter] = (char*)malloc(1025);
				strcpy(rStruct->sharedArray[*rStruct->sharedArrayCounter], domainName);
				printf("Requestor put domainName: %s\n", domainName);
				*rStruct->sharedArrayCounter+= 1;
				pthread_cond_signal(&reader); //just added
				pthread_mutex_unlock(&rStruct->sharedArrayLock);
			}
			else{
				printf("Requestor Sleeping\n");
				pthread_cond_signal(&reader);
				pthread_cond_wait(&writer, &rStruct->sharedArrayLock);
				pthread_mutex_unlock(&rStruct->sharedArrayLock);
				printf("WRITER HAS AWOKEN\n");
			}
		}

		fclose(currentFile);
		localServiced++;

	}
	pthread_mutex_lock(&rStruct->servicedFileLock);
	servicedFile = fopen(rStruct->servicedFile,"a");
	if (servicedFile == NULL) {
  		fprintf(stderr, "Can't open serviced file!\n");
  		exit(1);
	}
	fprintf(servicedFile, "Thread %u serviced %d file(s)\n", threadID, localServiced);

	fclose(servicedFile);
	pthread_mutex_unlock(&rStruct->servicedFileLock);

	printf("Thread %u just outside loop\n", threadID);
	pthread_exit(0);
}

void* resolver(struct resolverStruct* resStruct){
	FILE* resultsOF;
	char firstIPstr[INET6_ADDRSTRLEN];
	char domainName[1025];

	printf("Reader is ALIVEEEEE\n");

	while(*resStruct->ALIVE){
		pthread_mutex_lock(&resStruct->sharedArrayLock);
		if (*resStruct->sharedArrayCounter < 1){
			if (!(*resStruct->ALIVE)){
				printf("Res. thread breaking");
				pthread_mutex_unlock(&resStruct->sharedArrayLock);
				break;
			}
			printf("Reader sleeping\n");
			pthread_cond_wait(&reader, &resStruct->sharedArrayLock);
			printf("READER HAS AWOKEN\n");
		}
		strcpy(domainName,resStruct->sharedArray[*(resStruct->sharedArrayCounter) - 1]);
		printf("Reading SharedCounter: %d\n", *(resStruct->sharedArrayCounter));
		printf("Resolver reading: %s domain.\n", domainName);
		*resStruct->sharedArrayCounter-=1;
		pthread_cond_signal(&writer);
		pthread_mutex_unlock(&resStruct->sharedArrayLock);

		pthread_mutex_lock(&resStruct->resultsFileLock);
		resultsOF = fopen(resStruct->resultFile, "a");
		if (resultsOF == NULL){
			fprintf(stderr, "Can't open results file!\n");
			exit(1);
		}

		if(dnslookup(domainName,firstIPstr, sizeof(firstIPstr)) == UTIL_FAILURE){
			fprintf(stderr, "dnslookup failed :( %s\n", domainName);
			 strncpy(firstIPstr, "", sizeof(firstIPstr));
		}

		fprintf(resultsOF, "%s,%s\n", domainName, firstIPstr);
		fclose(resultsOF);
		pthread_mutex_unlock(&resStruct->resultsFileLock);
	}

	printf("Resolver outside loop\n");
	pthread_exit(0);
}



int main (int argc, char *argv[]) {

	if (argc > 15 || argc < 6) {
		printf("Invalid arguement entry. Format is: \n");
		printf("./multi-lookup <requester threads> <resolver threads> <result.txt> <serviced.txt> <..inputfiles..>\n");
		return 0;
	}

	int sharedCounter = 0;
	int ALIVE = 1;
	char* inputFiles[argc - 5]; //array for input files
	char* servicedFile = argv[4]; // servicedfile to write to
	char* resultFile = argv[3]; // resultsFile to write to
 	int reqThreadNum = atoi(argv[1]); //get Requestor thread #
	int resThreadNum = atoi(argv[2]); //get Resolver thread #
	pthread_t rthread[reqThreadNum]; // requestor threads entered
	pthread_t resThread[resThreadNum]; //resolver threads

	char *sharedArray[ARRAYSIZE];

	//Checking limits
	if (reqThreadNum > 5){
		reqThreadNum = 5;	
	}
	if (resThreadNum > 10){
		resThreadNum = 10;
	}

	//loop through inputfiles and add them to inputFiles array
	for(int i=0; i < (argc - 5); i++){
		inputFiles[i] = argv[5+i];
	}

	struct requestStruct Requestor; //initialize Requestor Struct
	struct requestStruct* RequestPtr;
	struct resolverStruct Resolver;	//initialize Resolver Struct
	struct resolverStruct* ResolverPtr;

	Requestor.inputFiles = inputFiles; //Requestor struct set to inputFiles array
	Requestor.servicedFile = servicedFile;
	Requestor.sharedArray = sharedArray; //Requestor struct set to sharedArray
	Requestor.sharedArrayCounter = &sharedCounter; //Requestor struct set to sharedCounter
	Requestor.totalIF = argc - 5;	//Total number of input files
	Requestor.totalServiced = 0;	//total inputFiles serviced initialized
	RequestPtr = &Requestor;	//Requestor struct pointer set to point to our Requestor

	Resolver.sharedArray = sharedArray; //Resolver struct set to sharedArray
	Resolver.sharedArrayCounter = &sharedCounter; //Resolver struct set to sharedCounter
	Resolver.resultFile = resultFile;
	Resolver.ALIVE = &ALIVE;
	ResolverPtr = &Resolver;	//Resolver struct pointer set to point to our Resolver

	pthread_mutex_init(&Requestor.servicedFileLock, NULL);
	pthread_mutex_init(&Requestor.servicedCountLock, NULL);
	pthread_mutex_init(&Requestor.inputFileLock, NULL);
	pthread_mutex_init(&Requestor.sharedArrayLock, NULL);

	pthread_cond_init(&writer, NULL);
	pthread_cond_init(&reader, NULL);

	pthread_mutex_init(&Resolver.sharedArrayLock, NULL);
	pthread_mutex_init(&Resolver.resultsFileLock, NULL);

	struct timeval start, end;
  	gettimeofday(&start, NULL);

	for (int i=0; i < reqThreadNum; i++){
		if(pthread_create(&rthread[i], NULL,  (void *)request, RequestPtr)){
			fprintf(stderr,"Error creating thread\n");
			return 1;
		}
	}

	//for loop for all resThreadNum
	for (int j=0; j < resThreadNum; j++){
		if(pthread_create(&resThread[j], NULL,  (void *)resolver, ResolverPtr)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}

	//join all req threads back together
	for(int i = 0; i < reqThreadNum; i++){
		if (pthread_join(rthread[i],NULL)){
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}

	ALIVE = 0;

	for(int j = 0; j < resThreadNum; j++){
		if (pthread_join(resThread[j],NULL)){
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}

	for (int i=0; i < 15; i++){
		printf("%d: %s\n", i,*(Requestor.sharedArray + i));
	}

	pthread_cond_destroy(&writer);
	pthread_cond_destroy(&reader);

	gettimeofday(&end, NULL);
  	printf("Runtime in seconds: %ld\n", (end.tv_sec - start.tv_sec));
	return 0;
}

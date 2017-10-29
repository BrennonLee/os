
#include "multi-lookup.h"

	//to compile program run gcc -o multi-lookup multi-lookup.c multi-lookup.h -lpthread

	//arg 2 - # of requester threads --> argv[1]
	//arg 3 - # of resolver threads --> argv[2]
	//arg 4 - results.txt	
	//arg 5 - serviced.txt
	//Then up to 10 input files.

//pthread_mutex_t servicedLock;


void* request(struct requestStruct* rStruct) {
	pid_t threadID = syscall(__NR_gettid);
	char domainName[100];
	
	//lock and open inputfile
	pthread_mutex_lock(&rStruct->servicedCountLock);
	rStruct->currentIF = fopen(rStruct->inputFiles[rStruct->totalServiced], "r+");
	if (rStruct == NULL){
		fprintf(stderr, "Can't open input file!\n");
	}
	rStruct->totalServiced += 1;
	pthread_mutex_unlock(&rStruct->servicedCountLock);
	
	printf("totalIF serived: %d and totalIF: %d\n", rStruct->totalServiced, rStruct->totalIF);
	while (rStruct->totalServiced <= rStruct->totalIF){
	printf("thread %u is inside the loop!\n", threadID);
		//while there is stuff to read in input file read it.
		 while(fscanf(rStruct->currentIF, "%s", domainName) == 1){
			pthread_mutex_lock(&rStruct->sharedArrayLock);
			printf("domainName: %s\n", domainName);
			pthread_mutex_unlock(&rStruct->sharedArrayLock);
		}	

		//lock and write to servicedfile
		pthread_mutex_lock(&rStruct->servicedFileLock);

		rStruct->servicedFile = fopen("serviced.txt","a");
		if (rStruct->servicedFile == NULL) {
	  		fprintf(stderr, "Can't open serviced file!\n");
	  		exit(1);
		}

		//fprintf(rStruct->servicedFile, "Thread %u serviced file(s): %s.\n", threadID, *(rStruct->inputFiles + 0));
		fprintf(rStruct->servicedFile, "Thread %u serviced file(s): %s.\n", threadID, *(rStruct->inputFiles + (rStruct->totalServiced - 1)));
	
		fclose(rStruct->servicedFile);
		pthread_mutex_unlock(&rStruct->servicedFileLock);
	
		//then update total serviced count
		pthread_mutex_lock(&rStruct->servicedCountLock);
		fclose(rStruct->currentIF);
		rStruct->totalServiced += 1;		
		if (rStruct->totalServiced <= rStruct->totalIF){
			rStruct->currentIF = fopen(rStruct->inputFiles[rStruct->totalServiced], "r");
			if (rStruct == NULL){
				fprintf(stderr, "Can't open input file!\n");
			}
		}
		pthread_mutex_unlock(&rStruct->servicedCountLock);
	}
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
	RequestPtr = &Requestor;	//Requestor struct pointer set to point to our Requestor struct

	Requestor.totalServiced = 0;	//total inputFiles serviced initialized

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


	//if (ifp == NULL) {
  		//fprintf(stderr, "Can't open input file!\n");
  		//exit(1);
	//}

	//char domainName[100];

	//while (fscanf(ifp, "%s", domainName) == 1) {
  	//	fprintf(servicedFile, "%s\n", domainName);
	//}
	//fclose(ifp);

	return 0;
}
#include <stdio.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>

int main(){

	int result;
	int number1 = 2;
	int number2 = 3;	

	syscall(333,number1, number2, &result);
	printf("Your result after running the adding system call is: %d\n",result );
	return 0;
}

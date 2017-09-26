/*
Written by Shivakant Mishra
Last update: September 18, 2017
*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

main(int argc, char *argv[])

{
  int i, c1, max = 100, status;
  FILE *fd;

  fd = fopen("squares", "w");
  for (i = 0; i < max; i++) {
    fprintf(fd, "%d\n", i * i);
  }
  fclose(fd);

  if ((c1 = fork()) == -1) /* Create the second child process */ {
    perror("Sqr: unable to fork child");
    exit(1);
  }
  else if (c1 == 0) {
    /* This is the third child process */
    printf("Third child pid = %d\n", getpid());
    execl("fourth", "fourth", NULL);
    perror("Third Child: execl failed");
    exit(1);
  }
  wait(&status);
}


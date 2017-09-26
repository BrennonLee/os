/*
Written by Shivakant Mishra
Last update: September 18, 2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


main()

{
  int max = 100, i, c1, c2, status;
  FILE *fd;

  if ((c1 = fork()) == -1) /* Create the first child process */ {
    perror("Master: unable to fork first child");
    exit(1);
  }
  else if (c1 == 0) {
    /* This is the first child process */
    printf("First child pid = %d\n", getpid());
    execl("sqr", "sqr", NULL);
    perror("First Child: execl failed");
    exit(1);
  }
  else {
    if ((c2 = fork()) == -1) /* Create the second child process */ {
      perror("Master: unable to fork second child");
      exit(1);
    }
    else if (c2 == 0) {
      /* This is the second child process */
      printf("Second child pid = %d\n", getpid());
      execl("cube", "cube", NULL);
      perror("Second Child: execl failed");
      exit(1);
    }
  }
  printf("master pid = %d\n", getpid());
  fd = fopen("whole_num", "w");
  for (i = 0; i < max; i++) fprintf(fd, "%d\n", i);
  fclose(fd);
  wait(&status);
  wait(&status);
}


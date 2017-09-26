/*
Written by Shivakant Mishra
Last update: September 18, 2017
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

main(int argc, char *argv[])

{
  int i, max = 100;
  FILE *fd;

  fd = fopen("fourths", "w");
  for (i = 0; i < max; i++) {
    fprintf(fd, "%d\n", i*i*i*i);
  }
  fclose(fd);
  sleep(60);
}

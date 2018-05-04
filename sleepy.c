/**
Jian Guo 47508738
File located on genuse26
Tried several times, the process changes.
**/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
  int count;
  //default as 5 if no time passed in, otherwise use atoi() to convert from char array to integer
  if (argc == 2)
    count = atoi (argv[1]);
  else
    count = 5;
  //if the count is 0 (time passed in is 0 or not digits), just return, no need to print anything
  if(count == 0)
    return 0;
  //begining of the sleep loop
  printf("%d-START\n", getpid());
  int i;
  for(i = 1; i <= count; i++ ){
    //everytime sleep 1 second, then print the TICK sentence
    sleep(1);
    printf("%d-TICK %d\n", getpid(), i);
   }
  return 0;
}

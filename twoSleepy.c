/**
Jian Guo 47508738
File located on genuse26
**/

#include <unistd.h>

#include <sys/types.h>

#include <stdlib.h>

#include <stdio.h>

#include <sys/wait.h>

/**
part I:
when getting into main method, the parent process is at running status.
when calling the fork() method, the parent process will create a child process, the child process is at the new status.
in the for loop, parent and child process switch from running to blocked, when the sleep() method is called, the running process turn to blcoked status.
when called exit() method, child and parent process will turn to exit status.

The parent procesa and child process could run evenly, my understanding is the sleep() method, it sleeps for 1 second while the print line takes way less
time than that, then the other process get the chance to execute, that's why we can see they are running in time sequence, 1 1 2 2 3 3, 
but there's no gurantee which process runs first.

**/

/**
part II:
Parent process runs first and exit before child process starts. 
The ppid of the child process is 1, these kind of processes are called orphan process since there parent process exit before they exit.
The child process is not notified right after the termination of parent process, the system simply redefines the parent of the child process
to the ancestor of every other process which is 1, so that's why the PPID os the child process is 1.
**/

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
  int f = fork();
  //begining of the sleep loop
  int i;
  int g;
  if(f == 0){
    g = fork();
  }
  for(i = 1; i <= count; i++ ){
    //everytime sleep 1 second, then print the TICK sentence
    if(f > 0){
      sleep(1);
      printf("%s%d%s%d%s%d\n", "Original Process with PID: ", (int)getpid(), " and PPID: ", (int)getppid(), " tick ", i);
    }else if(f == 0){
      if(g > 0){
        sleep(1);
        printf("%s%d%s%d%s%d\n", "Child Process with PID: ", (int)getpid(), " and PPID: ", (int)getppid(), " tick ", i);
      }else if(g == 0){
        sleep(1);
        printf("%s%d%s%d%s%d\n", "Grandchild Process with PID: ", (int)getpid(), " and PPID: ", (int)getppid(), " tick ", i);
      }
    }
  }
  if(f == 0){
    if(g == 0){
      printf("%s\n", "Grandchild Process Terminating");
      exit(0);
    }else if(g > 0){
      if(wait(&g) >= 0){
        printf("%s\n", "Child Process Terminating");
        exit(0);
      }
    }
  }else if(f > 0){
    if(wait(&f) >= 0){
      printf("%s\n", "Parent Process Terminating");
      exit(0);
    }
  }
  return 0;
}





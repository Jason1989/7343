//
//  msgQueue.c

//  Author: Jian Guo
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <unistd.h>

extern int errno;       // error NO.

int numCons;

int N;

struct message {
   int mtype;
   int text;
} msg;

struct threadParams {
   int id;
   int n;
};

/* Consumer Thread */
void *consumer(void  *temp) {
   struct threadParams * param = (struct threadParams*)temp;
   int total = 0;
   int id = param->id;
   int n = param->n;
   for(int i = 0; i < n; i++){
      //Read a message containing a value, C, consumed from the queue.
      int key = ftok("msgQueue.c", 53);
      int msgqid1 = msgget(key, 0666|IPC_CREAT);
      int rc = msgrcv(msgqid1, &msg, sizeof(msg.text), 0, 0); 
      if (rc < 0) {
         perror( strerror(errno) );
         printf("msgrcv failed, rc=%d\n", msgqid1);
         exit(0);
      } 

      //Add C to a running total maintained by each consumer thread.
      total += msg.text;

      //Display the string “Consumer thread Z consumed a C” where Z is the thread number – 0,1 or 2.
      printf("Consumer thread %d consumed a %d\n", id, msg.text);

      //Put the consumer thread to sleep for 1-3 seconds 
      int time = (rand()%3)+1;
      sleep(time);
   }
   printf("Total consumed by consumer thread %d = %d\n", id, total);
   return NULL;
}

int main(int argc, char *argv[]) {

   if(argc == 3) {
      N = atoi(argv[1]); 
      numCons = atoi(argv[2]); 
   }else if(argc == 2){
      //if no N value passed in, default as 10
      N = 10;
      numCons = atoi(argv[1]); 
   }else{
      N = 10;
      numCons = 3; 
   }
   // create a message queue. If here you get a invalid msgid and use it in msgsnd() or msgrcg(), an Invalid Argument error will be returned.
   int key = ftok("msgQueue.c", 53);
   int msgid = msgget(key, 0666|IPC_CREAT);
   if (msgid < 0) {
      perror(strerror(errno));
      printf("failed to create message queue with msgqid = %d\n", msgid);
      return 1;
   }

   //Thread ID
   pthread_t tid[numCons];   

   pid_t pID = fork();
   if (pID == 0){// child
      int res = N;

      int alloc[numCons];
      for(int i = 0; i < numCons; i++){
         alloc[i] = 0;
      }
      while(res > 0){
         for(int i = 0; i < numCons; i++){
            if(res > 0){
               alloc[i] = alloc[i] + 1;
               res = res - 1;
            }
         }
      }
      struct threadParams params[numCons];

      // Create the consumer threads */
      for(int i = 0; i < numCons; i++) {
         params[i].id = i;
         params[i].n = alloc[i];
         pthread_create(&tid[i],NULL,consumer,&params[i]);
      }
      for(int i = 0; i < numCons; i++)
         pthread_join(tid[i], NULL);
   } else if (pID < 0){ // failed to fork
      printf("failed to fork");
      exit(1);
      // Throw exception
   } else { // parent
      
      //running total of random number
      int total = 0;

      // Code only executed by parent process
      for(int i = 0; i < N; i++) {
         //Generate a random number, R, in the range of 0-999.
         int rNum = rand()%1000;
         struct message mm;
         //Send a message containing R.
         mm.mtype = 1;
         mm.text = rNum;
         int key = ftok("msgQueue.c", 53);
         int rc = msgsnd(msgid, &mm, sizeof(mm.text), 0); // the last param can be: 0, IPC_NOWAIT, MSG_NOERROR, or IPC_NOWAIT|MSG_NOERROR.
         if (rc < 0) {
            perror( strerror(errno) );
            printf("msgsnd failed, rc=%d\n", msgid);
            return 1;
         } 
         //Add R to a running total of all values produced.
         total += rNum;

         // Display the string “Producer produced a R”.
         printf("producer produced a %d\n", rNum);

         //Put the producer to sleep for 0-1 seconds.
         sleep(rand()%2);

      }

      //After the N iterations have completed display the string "Total produced = XXXX” (where XXXX is the sum of all R values produced) 
      //and wait for the child to terminate. It is the parent’s responsibility to create and destroy the queue.
      wait(NULL); // wait all child process terminated
      msgctl(msgid, IPC_RMID, NULL); // destroy the message queue
      printf("Total produced = %d\n", total);
      exit(0);
   }
   exit(0);
}

/*
1 for synchronization:

we use message queue to send and receive message, the key point here is the method msgrcv.
If there was no message on the queue, the msgrcv() call would have blocked our process until one of the following happens:
1.a suitable message was placed on the queue.
2.the queue was removed (and then errno would be set to EIDRM).
3.our process received a signal (and then errno would be set to EINTR.
From our program, we are guranteed each consumer will have enough message to read from the message queue, 
so if producer produced slower than the consumption, consumer will wait for the message to be produced
(Verified with seting producer sleep for 10seconds after each message generated, consumer will wati till the message produced then consume it).

2 for mutual exclusion

each thread has unique message type, so no two threads would race for the same message, thus there won't be race condtion, that also implements the 
mutual exclusion.
*/
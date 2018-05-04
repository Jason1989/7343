#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_LEN 30

extern char **environ;


int comparator (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

struct readThreadParams {
	char *filename;
	int size;
};

void* work(void* arg)
{
    char *p = (char*) arg;
    printf("threadpool callback fuction : %s.\n", p);
    sleep(1);
}


void write_to_file (struct readThreadParams *context) {
	struct readThreadParams *readParams = context;
	char *filename = readParams->filename;
	int size = readParams->size; 
	FILE *fp = fopen(filename, "w+");
	//generate random numbers and write to the file
	for(int i = 0; i < size; i++){
		fprintf(fp, "%d\n", rand());	
	}
	fclose(fp);  
	pthread_exit(0);
}

void sort_file (struct readThreadParams *context) {
	struct readThreadParams *readParams = context;
	char *filename = readParams->filename;
	FILE *fp = fopen(filename, "r");
	int index = 0;
	int size = 0;
	int num,r;
	//first loop to get the total line nubmers of the file
	r = fscanf(fp, "%d\n", &num);
	while (r != EOF)
	{
		size++;
		r = fscanf(fp, "%d\n", &num);
	}
	fseek(fp, 0, SEEK_SET);
	//using the size to create a new array to save all numbers
	int nums[size];
	r = fscanf(fp, "%d\n", &num);
	//second loop to put number into array
	while (r != EOF)
	{
		nums[index] = num;
		index++;
		r = fscanf(fp, "%d\n", &num);
	}
	fclose(fp);
	//sort array
	qsort(nums, index, sizeof(int), comparator);
	//open file again with write authority to write the sorted array to the file
	fp = fopen(filename, "w+");
	for(int i = 0; i < index; i++){
		fprintf(fp, "%d\n", nums[i]);	
	}
	fclose(fp);  
	pthread_exit(0);
}

int main (int argc, char *argv[])
{
	char cmd[MAX_LEN];
	char bak[MAX_LEN];
	char ** env = environ;    
	srand(time(NULL));   
	while (true)
	{
		printf ("%s","This is Jian Guo's shell \nPlease enter a command: ");
		//get the whole input including the spaces
		scanf ("%[^\n]%*c", cmd);
		//get the first word from the input
		char *token = strtok(cmd, " ");
		if(strcmp(token, "clr") == 0){
			system("clear");
		}else if(strcmp(token, "dir") == 0){
			system("ls -l");
		}else if(strcmp(token, "quit") == 0){
			exit(0);
		}else if(strcmp(token, "environ") == 0){
			while (*env) printf("%s\n",*env++); 
		}else{
			//if the command is less than 5 or longer than 5 and not in the previous commands
			//just execute them using system function
			if(strlen(token) < 5){
				system(token);
			}else{
				if(strcmp(token, "frand") == 0){

					pthread_t newthread;
					//get filename and size
					char *filename = strtok(NULL, " ");
					//if input is invalid, size will be 0
					int size = atoi(strtok(NULL, " ")); 

					struct readThreadParams readParams;
					readParams.filename = filename;
					readParams.size = size;

					pthread_create (&newthread, NULL, work, "10");
				}else if(strcmp(token, "fsort") == 0){

					pthread_t newthread;

					char *filename = strtok(NULL, " ");

					struct readThreadParams readParams;
					readParams.filename = filename;
					//check if the filename is valid, if not prompt error info and no need to create new thread

					FILE *fp = fopen(filename, "r");
					if (fp == NULL){
						printf("%s\n", "File not found, please check the name and path!");
				    }else{
						pthread_create (&newthread, NULL, work, "10");
				    }   
				}else{
					system(token);
				}
			}
		} 
	}
	return 0;
}


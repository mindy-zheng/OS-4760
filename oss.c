#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <time.h>
#include <stdbool.h>

// Process Control Block struct
typedef struct PCB {
        int occupied;           // either true or false
        pid_t pid;              // process id of the child
        int startSeconds;       // time when it was forked
        int startNano;          // time when it was forked
} PCB;

// Simulated system clock struct  
typedef struct Clock { 
	int seconds; 
	int nanoseconds; 
} Clock; 

// Function to display help message
void help() {
        printf("oss [-h] [-n proc] [-s simul] [-t timelimit] [-f logfile]");
        printf("\n\t Help Menu \n");
        printf("     -----------------\n");
	printf(" -h option: displays help message\n");
	printf(" -n [proc]: number of total children to launch\n");
	printf(" -s [simul]: indicates how many children can run simultaneously\n");
	printf(" -t [timelimit]: bound of time that a child process will be launched for from 1 to -t\n");
	printf("-f [logfile]: where you should write the output of oss");
}

#define SH_KEY 203100 


int main() { 
	// Start with initializing shared memory space 
	int shm_id = shmget(SH_KEY, sizeof(Clock), IPC_CREAT | 0666); 
	if (shm_id <= 0) { 
		fprintf(stderr, "Shared memory get failed\n"); 
		exit(1); 
	} 
	
	//Attach Clock struct to shared memory 
	// oss: increments clock; worker: checks clock periodically
	Clock *clock_ptr = (Clock*) shmat(shm_id, NULL, 0); 
	if (clock_ptr <= 0) { 
		fprintf(stderr, "Shared memory attach failed\n"); 
		exit(1); 
	} 
	
	// Initializing clock 
	clock_ptr -> seconds = 0; 
	clock_ptr -> nanoseconds = 0; 

	printf("Clock Initialized. Time: %d seconds and %d nanoseconds\n", clock_ptr->seconds, clock_ptr->nanoseconds); 




	// Detatch from shared memory 
	if (shmdt(clock_ptr) == -1) { 
		fprintf(stderr, "Detatching memory failed\n"); 
		exit(1); 
	}

	// Free shared memory segment 
	// shmctl(shm_id, IPC_RMID, NULL); 

	return 0; 
} 

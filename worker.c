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

//Clock Struct 
typedef struct Clock { 
	int seconds; 
	int nanoseconds; 
} Clock; 

#define SH_KEY 203100

int main() { 
	// Initialize shared memory space
	int shm_id = shmget(SH_KEY, sizeof(Clock), 0666); 
	if (shm_id <= 0) { 
		fprintf(stderr, "Shared memory get failed\n"); 
		exit(1); 
	} 
	// Worker: checks clock periodically to terminate or continue
	Clock *clock_ptr = (Clock*) shmat(shm_id, NULL, 0); 
	if (clock_ptr <= 0) { 
		fprintf(stderr, "Shared memory attach failed\n"); 
		exit(1); 
	} 

	//Testing to see if shared memory space works 
	printf("Time: %d seconds and %d nanoseconds\n", clock_ptr->seconds, clock_ptr->nanoseconds);

	//Detatching from shared memory space
	if (shmdt(clock_ptr) == -1) { 
		fprintf(stderr, "Detatching memory failed"); 
		exit(1); 
	} 	
	


	return 0; 

}



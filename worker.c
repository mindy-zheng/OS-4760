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

int main(int argc, char** argv) { 
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

//	printf("Seconds: %s, Nanoseconds: %s\n", argv[1], argv[2]); // Making sure exec works 
	int t_seconds = atoi(argv[1]);
	int t_nanoseconds = atoi(argv[2]); 
	if (clock_ptr->seconds > t_seconds || (clock_ptr->seconds == t_seconds && clock_ptr->nanoseconds >= t_nanoseconds)) { 
		printf("Termination time reached: %d seconds, %d nanoseconds\n", clock_ptr->seconds, clock_ptr->nanoseconds);
        exit(EXIT_SUCCESS);  // Terminate the process with a success status
    } else { 
		printf("Termination code not meeting"); 
	} 


	//Detatching from shared memory space
	if (shmdt(clock_ptr) == -1) { 
		fprintf(stderr, "Detatching memory failed"); 
		exit(1); 
	} 	
	


	return 0; 

}



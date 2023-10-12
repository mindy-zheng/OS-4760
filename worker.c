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
#include <sys/msg.h>

// Message queue struct
typedef struct msgbuffer {
    long mtype;
    int intData;
} msgbuffer;

#define PERMS 0644

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
	
	msgbuffer buf; 
	buf.mtype = 1;  
	int msgid = 0; 
	key_t msgkey; 

	// Get a key for our message queue 
	if ((msgkey = ftok("msgq.txt", 1)) == -1) {
		perror("ftok");
        exit(1);
    } 
	// Create message queue 
	if ((msgid = msgget(msgkey, PERMS)) == -1) {
		perror("msgget in child");
		exit(1);
	}

	
	// printf("Child %d has access to the queue\n",getpid());
	

	//	printf("Seconds: %s, Nanoseconds: %s\n", argv[1], argv[2]); // Making sure exec works 

	int t_seconds = atoi(argv[1]) + clock_ptr-> seconds; 
	int t_nanoseconds = atoi(argv[2]) + clock_ptr-> nanoseconds;
	int elapsed_seconds = 0;  
	int start_seconds = clock_ptr -> seconds; 

	while (1) { 
		printf("WORKER: About to receive a message from parent %d.\n", getpid());  // Print statement before msgrcv() 
		if (msgrcv(msgid, &buf, sizeof(msgbuffer), getpid(), 0) == -1) { 
			perror("msgrcv failure"); 
			exit(1); 
		} 
	    printf("Child %d has recieved message: %d\n", getpid(), buf.intData);
		printf("WORKER PID:%d PPID:%d Called with oss: TermTimeS: %d TermTimeNano: %d\n", getpid(), getppid(), t_seconds, t_nanoseconds);
		printf("--Received message\n");
		printf("Current time: %d seconds, %d nanoseconds\n", clock_ptr->seconds, clock_ptr->nanoseconds);
		printf("Termination time: %d seconds, %d nanoseconds\n", t_seconds, t_nanoseconds);
		
		if (clock_ptr->seconds > t_seconds || (clock_ptr->seconds== t_seconds && clock_ptr->nanoseconds >= t_nanoseconds)) { 
			buf.intData = 0; 
			printf("Terminating....\n"); 
			break; 
		} else { 
			buf.intData = 1; 
			printf("Continuing...\n");  
		}  

		buf.mtype = getppid(); // send Parent PID
		printf("About to send a message to parent %ld.\n", buf.mtype); 
    	if (msgsnd(msgid, &buf, sizeof(msgbuffer) - sizeof(long), 0) == -1) {
        	perror("msgsnd");
        	exit(1);
    	} 
		printf("Successfully sent message to parent %ld.\n:", buf.mtype); // debugging statement

		if (clock_ptr->seconds - start_seconds > elapsed_seconds) {
        	elapsed_seconds = clock_ptr->seconds - start_seconds;
        	printf("WORKER PID:%d PPID:%d SysClockS: %d SysclockNano: %d TermTimeS: %d TermTimeNano: %d\n", getpid(), getppid(), clock_ptr->seconds, clock_ptr->nanoseconds, t_seconds, t_nanoseconds);
        	printf("--%d seconds have passed since starting\n", elapsed_seconds);
    	}
	}
	

	//Detatching from shared memory space
	if (shmdt(clock_ptr) == -1) { 
		fprintf(stderr, "Detatching memory failed"); 
		exit(1); 
	} 	
	


	return 0; 

}



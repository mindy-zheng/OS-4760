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
	char strData[100]; 
	int intData; 
} msgbuffer; 

#define PERMS 0644

// Process Control Block struct
typedef struct PCB {
        int occupied;           // either true or false
        pid_t pid;              // process id of the child
        int startSeconds;       // time when it was forked
        int startNano;          // time when it was forked
} PCB;

PCB processTable[20]; 

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
		printf("-f [logfile]: where you should write the output of oss\n");
}

#define SH_KEY 203100 
#define ONE_SEC_IN_NANO 1000000000 // 1,000,000,000 nanoseconds = 1 second
#define INCREMENT_NANO 10000000 // 100,000,000 nanoseconds = 0.1 second
#define NANO_MAX 999999999 // for the upper bound in my random num generator 

void incrementClock(Clock *clock_ptr) {
	clock_ptr-> nanoseconds += INCREMENT_NANO;
		// Checks to see if nanoseconds has reached 1 second 
		if (clock_ptr-> nanoseconds >= ONE_SEC_IN_NANO) { 
			clock_ptr-> seconds += 1; // Increment 1 sec 
			clock_ptr-> nanoseconds -= ONE_SEC_IN_NANO; // restart nanoseconds to 0
		}
}	

void printPCB(PCB *processTable, Clock *clock_ptr) { 
	printf("OSS PID:%d SysClockS: %d SysClockNano: %d\n", getpid(), clock_ptr->seconds, clock_ptr->nanoseconds); 
	printf("Process Table: \n");
	printf("%-3s %-7s %-7s %-5s %-5s\n", "Entry", "Occupied", "PID", "StartS", "StartN"); 
	for (int i = 0; i < 20; i++) { 
		printf("%-3d %-8d %-8d %-5d %-5d\n", i, 
               processTable[i].occupied,
               processTable[i].pid,
               processTable[i].startSeconds,
               processTable[i].startNano);
    }
}

int randomNum(int min, int max) {
	return min + rand() / (RAND_MAX / (max-min+1) + 1);  
} 

int main(int argc, char **argv) { 
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
	// Testing shared memory and initialization
	//printf("Clock Initialized. Time: %d seconds and %d nanoseconds\n", clock_ptr->seconds, clock_ptr->nanoseconds); 

	// Set up my message queue  
	msgbuffer buf0, buf1; 
	int msgid; 
	key_t msgkey; 
	system("touch msgq.txt"); 
	
	// get key for our message queue 
	if ((msgkey = ftok("msgq.txt", 1)) == -1) { 
		perror("ftok"); 
		exit(1); 
	}

	// create message queue: 
	if ((msgid = msgget(msgkey, PERMS | IPC_CREAT)) == -1) { 
		perror("msgget in parent"); 
		exit(1); 
	}
	
	printf("Message queue sucessfully set up\n"); 

	// Command line arguments: 
	int opt;
	int processes, simul, timelimit; 
	char *logfile = NULL; 
	
	while ((opt = getopt(argc, argv, "hn:s:t:f:")) != -1) { 
		switch (opt) { 
			case 'h':
				help(); 
				break;
			case 'n': 
				processes = atoi(optarg); 
				break; 
			case 's': 
				simul = atoi(optarg); 
				break; 
			case 't': 
				timelimit = atoi(optarg); 
				break; 
			case 'f': 
				logfile = optarg; 
				break;
			default:
				help(); 
				exit(EXIT_FAILURE); 
		}
	}
	
	// Bound from 1 to -t. Random number generated within bounds: 
	srand(time(0)); 
	int random_seconds = randomNum(1, timelimit); // Random seconds 
	int random_nanoseconds = randomNum(0, 999999999); // Random nanoseconds 
	// Change to strings for execlp 
	char str_seconds[10], str_nanoseconds[10]; 
	sprintf(str_seconds, "%d", random_seconds); 
	sprintf(str_nanoseconds, "%d", random_nanoseconds); 

	// printf("Timelimit: 1 to %d, random generated num: %d\n", timelimit, printf("Timelimit: 1 to %d, random generated num: %d\n", ti); 
	// printf("Timelimit: 1 to %d, random generated num: %d\n",  random_t);
	

	// Testing command parse options
	//printf("Processes = %d, simultaneous processes = %d, timelimit = %d, logfile = %s\n", processes, simul, timelimit, logfile);
	


	// Creating child processes in accordance to command arguments 
	int num_processes = 0; // number of child processes running 
	int total_processes = 0; // total child processes created 
	int max_processes = processes; 
	int max_simul = simul; 
	// loop to create ammount of child processes need and simultaneous processes 
	while (total_processes < max_processes) { 
		incrementClock(clock_ptr); // Increment clock
		//printf("Clcok has been incremented: %d seconds, %d nanoseconds\n", clock_ptr-> seconds, clock_ptr->nanoseconds); // Debug statement to see if the clock is incrementing correctly
		if (num_processes < max_simul) { 
			pid_t pid = fork(); // create child process 

			if (pid == -1) {
				fprintf(stderr, "Forking failure"); 
				exit(EXIT_FAILURE); 
			} 

			// Child Process
			if (pid == 0) { 
				// executes the worker program
				execlp("./worker", "./worker", str_seconds, str_nanoseconds, NULL);
				perror("execlp");
				exit(EXIT_FAILURE); 
			 
				//printf("This is the child process: %d", getpid()); 
			} else { 
				num_processes++; // increments total child processes currently running
				total_processes++; // increments total child processes created
				printf("Child process created: %d. Total processes: %d. Running processes: %d.\n", pid, total_processes, num_processes);	

				// Find empty spot in table 
				int i; 
				for (i = 0; i < 20; i++) {
					if (!processTable[i].occupied) {
						break; 
					}
				}
				
				// Add processes to table: 
				if (i < 20) { 
					processTable[i].occupied = 1; 
					processTable[i].pid = pid;
					processTable[i].startSeconds = clock_ptr-> seconds;
					processTable[i].startNano = clock_ptr-> nanoseconds; 
					printf("Process added to PCB - %d\n", pid);

					// Display table 
					printPCB(processTable, clock_ptr);  
				} 
			}
				 int status;
				 pid_t wpid;
					while ((wpid = waitpid(-1, &status, WNOHANG)) > 0) {
                        if (WIFEXITED(status)) {
                            printf("Child with PID %d exited with status %d\n", wpid, WEXITSTATUS(status));
                            num_processes--; // decrement the number of running processes
							printf("Process terminated: %d. Running processes: %d.\n", wpid, num_processes);
                    }
                }


		}
	}




	// Detatch from shared memory 
	if (shmdt(clock_ptr) == -1) { 
		fprintf(stderr, "Detatching memory failed\n"); 
		exit(1); 
	}

	// Free shared memory segment 
	shmctl(shm_id, IPC_RMID, NULL); 


	return 0; 
} 

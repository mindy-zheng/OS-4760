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

int main() { 

	return 0; 
} 

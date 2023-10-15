# OS-4760
Mindy Zheng

This is project 3 for CS 4760.

As I struggled with the previous project, I want to make sure that I approach this project with a thorough understanding of what it's asking and intending to accomplish. 

The brief synopsis of this project essentially is that there are 2 executables: oss which handles launching a certain number of worker processes and coordinating their execution utilizing the message queue. On the other hand, the worker processes creates child processes that interact with the oss through message queue.

I first took care of the main elements of this project. I made sure that my memory space that I initialized worked through oss.c and worker.c. I noticed that it only worked if I didn't detatch and free up my memory space before worker.c could get to it. Initially, I kept getting an error that my worker.c wasn't accessing the shared memory correctly. This project, I meticulously implemented many debugging safeguards so I didn't get frustrated with early mistakes. Next, I made a function to increment the clock and set up my process table. 

I ran into my first problem when I forgot to implement waitpid. The children would never terminate, thus the worker clock wasn't incrementing. It was a small mistake, but I was able to fix it and make sure that my clock was incrementing the way I wanted it to and that the children could view the correct time. 

LOG: 
10/10 - I have finally set up a loop that will launch appropriate number of processes (add more error checks for the command line argument), set up a PCB table and correctly implemented something to add process into table, initialized and incremented the clock, made sure the children can access shared memory and view it. 
Goals: Next, I need to create a message buffer system that will send and recieve messages and terminate accordingly. This can take the next full day.    
  
10/11 - I was running into a problem where I thought the condition I implemented for the children to terminate wasn't working. I spent a while figuring out what was happening before I realized.. I included a print statement in my increment clock function to see if it was incrementing the values correctly and never removed it. 
I have created a function to displ ay the PCB table. So right now, I have the general essence of project 2 finished. Now, I have to conquer the message queue thing. So I'm going to remove the condition in my worker process that terminates the program when the termination time is finished. I need to create the message queeu in my oss, send a message from my oss, recieve the message in my worker, check the clock, respond, and then recieve messages in my oss.

I have now successfully set up and implemented the message queues in both programs. From here, I'll work on the logic and then make sure I have functionality for the logfile command argument.  

10/12 - I have run into a problem. I've kept up with the debugging statements so I can trace what's happening, but I'm a bit confused. Here is the last run for picking up where I left off. 
zheng@opsys:~/zheng.3$ ./oss -n 6 -s 2 -t 5
Prepping to fork...
Child process created: 3345386. Total processes: 1. Running processes: 1.
About to send a message to child 3345386.
OSS send message 1 to child: 3345386
WORKER: About to receive a message from parent 3345386.
Child 3345386 has recieved message: 1
WORKER PID:3345386 PPID:3345380 Called with oss: TermTimeS: 2 TermTimeNano: 494897100
--Received message
Current time: 0 seconds, 10000000 nanoseconds
Termination time: 2 seconds, 494897100 nanoseconds
Continuing...
About to send a message to parent 3345380.
Successfully sent message to parent 3345380.
:WORKER: About to receive a message from parent 3345386.
Prepping to receive a message from child 3345386.
^C

10/12 Okay, I've fixed the problem. 
I had a variable being decremented when it shouldn't have been; I believe it was the total number of processes.. My message queue functionalitys were messed up. Even though it was successful in setting up, I had the logic of it wrong and not fleshed out. 


10/12 night - So, I currently have implemented a common C I/O for wriitng into a logfile. It seems to be printing out messages fine, with the exception of a few errors. This is where i'll pick up on tomorrow. 

1. I keep getting a shared memory get failed messsage, which I'm very unsure of . 
2. I also need to monitor the logfile more closely. I didn't get a "terminating" line in my log file. I might need to implement this writing in the log in my WAITPID function. 
3. I need to implement a signal to kill all processes and free up memory after 60 seconds. 
4. I still need to find a way to print the PCB table every .5 seconds. 

10/13 - 
After debugging more, I found that my message queue is being removed before all the worker processes have finishing sending their messages. Figured this out when suddenly started getting error messages: "failure to send message:invalid argument" errors. So I need to modify and wait for termination message... 
 

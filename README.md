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

Now that I have that fixed, I am really happy where I am. 

# OS-4760
Mindy Zheng

This is project 3 for CS 4760.

As I struggled with the previous project, I want to make sure that I approach this project with a thorough understanding of what it's asking and intending to accomplish. 

The brief synopsis of this project essentially is that there are 2 executables: oss which handles launching a certain number of worker processes and coordinating their execution utilizing the message queue. On the other hand, the worker processes creates child process that interact with the oss through message queue.

I first took care of the main elements of this project. I made sure that my memory space that I initialized worked through oss.c and worker.c. I noticed that it only worked if I didn't detatch and free up my memory space before worker.c could get to it. Initially, I kept getting an error that my worker.c wasn't accessing the shared memory correctly. This project, I meticulously implemented many debugging safeguards so I didn't get frustrated with early mistakes. Next, I made a function to increment the clock and set up my process table. 

I ran into my first problem when I forgot to implement waitpid. The children would never terminate, thus the worker clock wasn't terminating correctly. It was a small mistake, but I was able to fix it and make sure that my clock was incrementing the way I wanted it to and that the children could view the correct time.  

  

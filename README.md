# System Programming Lab 11 Multiprocessing 
# Overview 
My program allows the number of processes to be entered into the command. This allows for the runtime to change accordingly.
# Results
[Screenshot 2025-11-14 151638.jpg](https://github.com/MSOE-CPE2600/multiprocessing-turney-moutheye/blob/main/Screenshot%202025-11-14%20151638.jpg)

This graph shows the clear relationship between the increasing amount of processes helping the runtime become shorter only up to a certain point where it begins to taper off.

# Lab 12 Multithreading
# Overview 
This program allows for the addition of multithredding along with the previous multiprocessing. This allows for the runtime to change drastically.
# Results
https://github.com/MSOE-CPE2600/multiprocessing-turney-moutheye/blob/lab12dev/Multithreading%20results.jpg 

After benchmarking the program running a differing amount of processes and threads, an interesting observation can be made. Having a high amount of processes and threads is not the most efficient. Rather the best time was given with 5 processes and 10 threads. This balance of having double the threads to the amount of processes is one that is common in processors in the real world showing that this result makes sense. It did seem like threads were slightly more effective vs processes. Since the processes can effectivly handle two threads at once it makes it more effective having many more threads to processors can slow it down since there is more overhead for the processors which can slow it down.



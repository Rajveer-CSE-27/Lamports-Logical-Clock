# Lamports-Logical-Clock

A Lamport logical clock is a numerical software counter value maintained in each process. Conceptually, this logical clock can be thought of as a clock that only has meaning in relation to messages moving between processes. When a process receives a message, it re-synchronizes its logical clock with that sender.

It is one of the crucial things in modern operating systems.
I have created c++ module to implement Lamport's Logical Clock.

Program should take as input a description of several process schedules (i.e., lists of send, receive or print operations). The output of your program will be a linearization of these events in the order actually performed, annotated with Lamport’s clock values.
The input of the program will be a collection of processes, each with a list of operations to perform.
The processes are named p1...pn for some n (you may assume that n is at most 10) The format of a
process is:
begin process p1
operation
...
operation
end process
where each line contains a basic operation. The possible basic operations are:
    ● send (pj, ..., pk) msg (that is, send message msg to all processes in the list (pj, ..., pk))  
    ● recv pN msg (that is, receive message msg from process pN)
    ● print msg (that is, print message msg to the terminal) where msg is any alphanumeric string.

To run the program you must enter :
      1. Number of processes.
      2. Functionalities of each process mentioned above
      3. "exit" string at the end.

Deadlock state is also handled if all the processes request at same time. For example, if after the occurrence of n events all processes are in deadlock, the output whill be showing those n events along with their clocks and then print “system deadlocked”.

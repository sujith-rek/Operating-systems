P.Soma Sujith
2110110400
P1
=========================================> Scheduling Algorithms <=========================================

1. First Come First Serve (fcfs.c)
2. Shortest Job First (sjf.c)
3. Round Robin (rr.c)
4. Priority Scheduling (priority.c)

Input : Should be given through input.txt file 
Input Format:

Every input will be of type int

fcfs and sjf -> 
_______________
first integer will be number of processes and 
next three integers should be pid, arrival time and burst time of every process

Sample Testcase;
4
1 0 3
2 1 2
3 2 6
4 2 2

rr ->
_____
first int will be number of processes and next int will be timeslice/time quantam
next n lines will be taking pid, arrival time and burst time in (int)

Sample Testcase:
4 2
1 0 3
2 1 2
3 2 6
4 2 2


priority -> Lowest priority number first
___________
first int is number of processes and then in every line,
pid, arrival time, burst time and priority number

Sample Testcase:
4
1 0 3 1
2 1 2 1
3 2 2 2
4 2 6 0

Output:
For every algorithm, output will be pids in order of Execution/completition



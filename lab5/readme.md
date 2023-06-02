P.Soma Sujith
2110110400
P1

Q1 ==> program to show the importance of locks
We use mutex lock and unlock to protect the critical section of the code. If we don't use mutex lock and unlock, then the output will be different everytime we run the program. But if we use mutex lock and unlock, then the output will be same everytime we run the program.
for non mutex part of the code, we get different output everytime we run the program.
sometimes it can be 10000 or nearto 10000

Q2a ==> Printing thread id without synchronization
the order of printing is random

Q2b ==> Printing thread id with synchronization
we use conditional wait to make created threads synchronize and print in the order of their ID


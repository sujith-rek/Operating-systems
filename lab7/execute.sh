#!/bin/bash
# Compiler: gcc
# Author: Sujith
# Roll No: 2110110400
# Practical batch : P1
# Date: 26/04/2023
# OS: Kali Linux

# If permission denied to execute the script, use the following command
# chmod +x execute.sh

# Compile Programs
gcc -o test0 test0.c pgm.c
gcc -o test1 test1.c pgm.c
gcc -o test2 test2.c pgm.c

# If Pthread warning is generated, uncomment the following and comment the above
# gcc -o test0 test0.c pgm.c -lpthread
# gcc -o test1 test1.c pgm.c -lpthread
# gcc -o test2 test2.c pgm.c -lpthread
# OR whichever works
# gcc -o test0 test0.c pgm.c -pthread
# gcc -o test1 test1.c pgm.c -pthread
# gcc -o test2 test2.c pgm.c -pthread

# Execute Programs
# Output is redirected to a file
# File is made writable before execution and made read-only after execution
# so that the output is not changed by the user
chmod +w t0.txt
./test0 > t0.txt
chmod -w t0.txt

chmod +w t1.txt
./test1 > t1.txt
chmod -w t1.txt

chmod +w t2.txt
./test2 > t2.txt
chmod -w t2.txt
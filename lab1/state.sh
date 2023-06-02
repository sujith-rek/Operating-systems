#!/bin/bash
i=0
rm state.txt
touch state.txt
while ((i<10))
do
	ps aux | grep "state.sh" >> state.txt
	((i++))
done

#!/bin/bash

lineCount=8000000
((filterProb=($lineCount*$2)/100))
filterCount=1

for (( counter = $lineCount; counter>0; counter-- ))
do
    ((tmp=1+($RANDOM % 100)))
    if [ $tmp -lt $2 ];
    then
        echo "short $filterCount" >> $1
        ((filterCount=$filterCount+1))
    else
        echo "This line is larger than 20 characters." >> $1
    fi
done

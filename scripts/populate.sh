#!/bin/bash

# This script is used to generate test files for filter experiment.
# No effort has been taken to make this robust.
#
# It takes 2 arguments
# 1. file that is to be created
# 2. filter predicate selectivity (less than 20 characters long)
#
# It creates a file with lineCount lines.
# Each line is less than 20 characters long with a 
# probabilty equal to the specified selectivity.
#
# We end up having a test data file with approximately given selectivity
# percentage of lines to satisfy the filter predicate (less than 20
# characters long). 
#
# Usage:
# ./populate.sh <fileName> <selectivityPercent>


lineCount=1000000
((filterProb=($lineCount*$2)/100))
filterCount=1

for (( counter = $lineCount; counter>0; counter-- ))
do
    ((tmp=1+($RANDOM % 100)))
    if [ $tmp -le $2 ];
    then
        echo "short $filterCount" >> $1
        ((filterCount=$filterCount+1))
    else
        echo "This line is larger than 20 characters." >> $1
    fi
done

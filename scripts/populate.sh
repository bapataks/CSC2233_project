#!/bin/bash

# This script is used to generate test files for filter experiment.
# No effort has been taken to make this robust.
#
# It takes 2 arguments
# 1. file that is to be created
# 2. filter predicate selectivity (less than 20 characters long)
#
# It creates a file with lineCount lines
# and probabilistically writes a line that is less than 20
# characters long with specified selectivity as probabilty
#
# We end up having a test file with less than given selectivity
# with respect to (less than 20 characters long) but also
# close to the given amount
#
# Usage:
# ./populate.sh <fileName> <selectivityPercent>


lineCount=1000000
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

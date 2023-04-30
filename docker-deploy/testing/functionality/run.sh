
make clean



make



#!/bin/bash

n=10

# Loop through the numbers from 1 to n
for ((i=1; i<=n; i++))
do
  ./main test$i.xml
done
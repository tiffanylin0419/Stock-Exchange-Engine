
#!/bin/bash

fileNum=10
maxThread=256
start=$(date +%s.%N)


for ((i=1; i<=maxThread; i++))
do
  for  ((j=1; j<=fileNum; j++))
  do
    ./main test$j.xml
  done
done


end=$(date +%s.%N)
runtime=$(echo "$end - $start" | bc)

echo "Runtime was $runtime seconds"

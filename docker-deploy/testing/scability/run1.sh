
#!/bin/bash
fileNum=10
maxThread=10

for ((i=1; i<=fileNum; i++))
do
    start=$(date +%s.%N)
    for ((j=1; j<=10; j++))
    do    
        ./main test$i.xml > /dev/null

    done
    end=$(date +%s.%N)
    runtime=$(echo "$end - $start" | bc)
    echo "$i :Runtime was $runtime seconds"
done




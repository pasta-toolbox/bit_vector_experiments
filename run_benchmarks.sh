#!/bin/bash

now=$(date)
file_name=results_$(date +'%s').txt

echo ${file_name}

lscpu | tee -a ${file_name}

for size in 1 2 3 4
do
    for fill_rate in 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95
    do
        echo "Running benchmark for size ${size}M and fill rate ${fill_rate}"
        ./build/rank_select_benchmark -b ${size}M -q 100M -f ${fill_rate} | tee -a ${file_name}
    done
done

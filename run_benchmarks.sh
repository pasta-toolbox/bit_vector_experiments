#!/bin/bash

now=$(date)
folder_name=results_$(date +'%s')
mkdir -p ${folder_name}

# Name of every non-cs-poppy algorithm contained in the benchmark. We
# use these names to filter the algorithms during the benchmark,
# because the cs-poppy implementations do not finish.
non_poppy_names="sdsl_rank_v5,sdsl_default_select,sdsl_default_rank,pasta_popcount_wide,pasta_popcount_flat,pasta_popcount,simple_select_half,simple_select,rank9_select"

# Name of the cs-poppy algorithms, that are separated for the reason
# described above.
poppy_names="poppy_rank_select_improved,poppy_rank_select"

file_name=${folder_name}/benchmarks.txt
lscpu | tee -a ${file_name}

# Run the benchmarks for our implementations and all other rank and
# select data structures than can handle the input sizes.
for rep in 1 2 3
do
    for size in 1 2 4 8 16 32
    do
        for fill_rate in 10 50 90
        do
            echo "Running benchmark for size ${size}G and fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${non_poppy_names} | tee -a ${file_name}
	        echo "Running benchmark for size ${size}G and adversarial fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${non_poppy_names} -a | tee -a ${file_name}
        done
    done
done
# Run the benchmarks for cs-poppy and cs-poppy-fs. However, we first
# run the benchmarks for the smaller inputs, because these inputs can
# be handled by the implementations.
for rep in 1 2 3
do
    for size in 1 2 4
    do
        for fill_rate in 10 50 90
        do
            echo "Running benchmark for size ${size}G and fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${poppy_names} | tee -a ${file_name}
	        echo "Running benchmark for size ${size}G and adversarial fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${poppy_names} -a | tee -a ${file_name}
        done
    done
done
# Now, we try to run the rest of the benchmarks with the poppy rank
# and select data structure. We to this in two separate loops, because
# this loop will not terminate within reasonable time.
for rep in 1 2 3
do
    for size in 8 16 32
    do
        for fill_rate in 10 50 90
        do
            echo "Running benchmark for size ${size}G and fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${poppy_names} | tee -a ${file_name}
	        echo "Running benchmark for size ${size}G and adversarial fill rate ${fill_rate}"
            ./build/rank_select_benchmark -b ${size}K -q 100 -f ${fill_rate} -n ${poppy_names} -a | tee -a ${file_name}
        done
    done
done

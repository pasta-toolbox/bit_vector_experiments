# Benchmark Scripts for pasta::bit_vector

This repository contains scripts that help to reproduce the results of the paper "Engineering Compact Data Structures for Rank and Select Queries on Bit Vectors".

To clone this repository and build the benchmark please use the following commands.
```
git clone https://github.com/pasta-toolbox/bit_vector_experiments --recursive
cd bit_vector_experiments
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build -j
```
This will compile all code needed for the benchmark in the `build` folder, which is used in the `run_benchmarks.sh` script.
Next, we can run the script.

```
./run_benchmarks.sh
```

This script will copy the [`template`](template/) folder and rename it to `results_CURRENTDATE`, where `CURRENTDATE` can be decoded using the `date` command (`date --date @CURRENTDATE`).
Within the folder, you find a LaTex-Document [plots.tex][] that contains all layout and styling information needed to recreate the plots.
To make the plots based on the newly created data, we rely on [sqlplot-tools][].
This tool reads the newly created `results.txt` file and passes all values directly into [plots.tex][].
Please refer to [sqlplot-tools]' website for information on how to install it.
If you have installed [sqlplot-tools] simply run `sqlplot-tools plots.tex` and compile [plots.tex][] to see the final results.

[plots.tex]: template/plots.tex
[sqlplot-tools]: https://github.com/bingmann/sqlplot-tools/

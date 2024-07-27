# Calypso: Cardinality Estimation for Super Spreader
This repository contains all related code of our paper "Calypso: Cardinality Estimation for Super Spreader
Detection via Skew-aware Flow Classification". 

## Introduction

Detecting super spreaders is crucial in network management, security monitoring, and epidemiology. Super spreaders
in computer networks, such as devices or IP addresses that gener-
ate high traffic volumes, can cause congestion and security issues.
Traditional flow size and cardinality measurement methods face
challenges due to the need to handle duplicate elements and
overlapping data. Existing techniques, such as Linear Counting
(LC), Multi-Resolution-Bitmap (MRB), and HyperLogLog++
(HLL), are efficient for single flows but inefficient for super
spreader detection across multiple flows. Calypso addresses these
challenges using dedicated HLL structures for large flows and
a shared LC structure for small flows, leveraging the Pareto
distribution to optimize storage space. This design allows for
efficient deduplication and intersection calculation between dif-
ferent data structures. Experimental results demonstrate that in
all scenarios, Calypso's F1 score is optimal. Calypso achieves an
F1 score up to 4.2 times higher than the best-compared algorithm
while maintaining comparable throughput.

## About this repo

- `include` contains codes of Calypso as well as other foundational algorithms. 

- `experiments` contains codes of Calypso and the related algorithms in cardinality measurement and super spreader detection.

- `dataset` contains two small dataset shards to run our tests. We also upload our synthetic datasets to google drive and leave a link here.

- More details can be found in the folders.

## Requirements

- CMake
- g++
- C++11

## How to run
first run the following command to generate a Makefile. 
```
$ mkdir build
$ cd build
$ cmake ..
```

Then run the following command to generate a executable file for each experiment.  
```
$ make
```

Finally, run the following command to generate the results. 
```
$ ./[program] [fileType] [filePath]
```
1. `[program]`: The executable program of different experiments.
2. `[fileType]`: An integer, representing the type of dataset:
- 0: IP Trace Dataset (CAIDA)
- 1: synthetic dataset
- 2: Other real-world datasets, such as Amazon, DBLP, and similar datasets, are also included.
3. `[filePath]`: The path of the dataset you want to run. 


The results will be shown on the screen. After the program completes the calculation, we wi

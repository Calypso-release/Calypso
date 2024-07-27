# Calypso: Cardinality Estimation for Super Spreader
This repository contains all related code of our paper "Calypso: Cardinality Estimation for Super Spreader
Detection via Skew-aware Flow Classification". 

## Introduction

Finding top-k items in data streams is a fundamental problem in data mining. Unbiased estimation is well acknowledged as an elegant and important property for top-k algorithms. In this paper, we propose a novel sketch algorithm, called WavingSketch, which is more accurate than existing unbiased algorithms. We theoretically prove that WavingSketch can provide unbiased estimation, and derive its error bound. WavingSketch is generic to measurement tasks, and we apply it to five applications: finding top-k frequent items, finding top-k heavy changes, finding top-k persistent items, finding top-k Super-Spreaders, and join-aggregate estimation. Our experimental results show that, compared with the state-of-the-art Unbiased Space-Saving, WavingSketch achieves 9.3¡Á faster speed and 10<sup>3</sup>¡Á smaller error on finding frequent items. For other applications, WavingSketch also achieves higher accuracy and faster speed.


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
- 2: Other real-world datasets, such as those from Amazon, DBLP, and similar sources, are also included.
3. `[filePath]`: The path of the dataset you want to run. 


The results will be shown on the screen. After the program completes the calculation, we wi

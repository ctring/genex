# ONEX

## Install dependencies

### macOS:

```bash
  brew install cmake
  brew install boost
  brew install readline
```

### Linux:
- Install [latest CMake for Linux](https://cmake.org/download/).
- Install [Boost 1.6 or above](http://www.boost.org/doc/libs/1_61_0/more/getting_started/unix-variants.html)
- Install readline library:
```bash
  sudo apt-get install libreadline6 libreadline6-dev
```

### Windows:

- Install [CMake for Windows](https://cmake.org/download/). Make sure that the **bin** directory of CMake is in your PATH environment variable.
- Install [MinGW Distro with Boost](https://nuwen.net/mingw.html) in **C:\\**
- Install [WinEditLine](https://sourceforge.net/projects/mingweditline/files/) in **C:\\**
- Copy and replace **C:\wineditline\include\editline\readline.h** with **readline.h** in this repository.

## Build and Run

### Linux & macOS
Change current directory to this repository and run the following commands
```bash
mkdir build
cd build
cmake ..
make
```

A binary named 'onex' will be produced in the build directory. To run it, use:
```bash
./onex
```

### Windows

Change current directory to this repository and run the following commands in **cmd**
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make
```

A binary named 'konex' will be produced in the build directory. To run it, use:
```bash
./konex
```

## CLI documentation

Use 'help' to see the details documentation of all commands.

## Dataset format

An acceptable dataset file contains a list of time series of equal length. Each data point of the time series can be separated by any character (space is used by default) as this can be set by using the 'separators' parameter of the 'load' command.

Values of the time series must be in the range of [0, 1]. A loaded dataset can be normalized to this range by using the 'normalize' command.

## Example usage

1. Load a comma separated dataset:

```
>> load ../datasets/test/test_15_20_comma.csv 0 0 ,
Dataset loaded
  Name:        ../datasets/test/test_15_20_comma.csv
  ID:          0
  Item count:  15
  Item length: 20
```

2. Normalize it:

```
>> normalize 0
Dataset 0 is now normalized
```

3. Group it with ST = 0.1 (for larger datasets, use larger ST for faster preprocessing):
```
>> group 0 0.1
Processing time series space of length 2
  Grouping progress... 57/285 (20%)
  Grouping progress... 114/285 (40%)
  Grouping progress... 171/285 (60%)
  Grouping progress... 228/285 (80%)
  Grouping progress... 285/285 (100%)
Command executed in 0.005535s
Dataset 0 is now grouped
Number of Groups: 2578
```

4. Load another dataset to use as queries:
```
>> load ../datasets/test/test_10_20_space.txt
Dataset loaded
  Name:        ../datasets/test/test_10_20_space.txt
  ID:          1
  Item count:  10
  Item length: 20
```

5. Find top 10 similar time series from dataset 0 to a time series in dataset 1 
```
>> kSim 10 0 1 0 0 10
Command executed in 0.002108s
Timeseries 7 [8, 19] - distance = 0.3933
Timeseries 0 [7, 18] - distance = 0.3955
Timeseries 5 [7, 18] - distance = 0.3958
Timeseries 5 [8, 19] - distance = 0.3965
Timeseries 5 [9, 20] - distance = 0.3974
Timeseries 7 [9, 20] - distance = 0.3988
Timeseries 5 [6, 17] - distance = 0.4006
Timeseries 3 [1, 12] - distance = 0.4013
Timeseries 7 [2, 13] - distance = 0.4033
Timeseries 4 [0, 11] - distance = 0.4034
```

## Acknowledgement

The ONEX codebase includes the trillionDTW[0] methods, uses the same pruning
methods, and even uses the trillion implementation of lemire lower/upper
bound.

## Reference

[0] http://www.cs.ucr.edu/~eamonn/SIGKDD_trillion.pdf

# GENEX

This is the source code for ...

## Install dependencies

### macOS:

```bash
  brew install cmake
  brew install boost
  brew install boost-python
  brew install readline
```

### Linux:
- Install [latest CMake for Linux](https://cmake.org/download/).
- Install [Boost 1.6 or above with Boost.Python](http://www.boost.org/doc/libs/1_61_0/more/getting_started/unix-variants.html)
- Install readline library:
```bash
  sudo apt-get install libreadline6 libreadline6-dev python-dev
```

### Windows (Not Tested):

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

A binary named 'genexcli' will be produced in the build directory. To run it, use:
```bash
./genexcli
```

### Windows

Change current directory to this repository and run the following commands in **cmd**
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make
```

A binary named 'genexcli' (Genex command-line interface) will be produced in the build directory. To run it, use:
```bash
./genexcli
```

## CLI documentation

Use 'help' to see the details documentation of all commands.

## Dataset format

An acceptable dataset file contains a list of time series of equal length. Each data point of the time series can be separated by any character (space is used by default) as this can be set by using the 'separators' parameter of the 'load' command.

Values of the time series must be in the range of [0, 1]. A loaded dataset can be normalized to this range by using the 'normalize' command.

## Example usage

1. Load a comma separated dataset. `target_ds` is an arbitrary name given to the dataset so that we can refer to it later.
```
>> load target_ds ../datasets/test/test_15_20_comma.csv ","
Dataset loaded
  Name:        ../datasets/test/test_15_20_comma.csv
  ID:          0
  Item count:  15
  Item length: 20
```

2. Normalize the dataset.
```
>> normalize target_ds
Dataset target_ds is now normalized
```

3. Group the dataset with ST = 0.1 (for larger datasets, use larger ST for faster preprocessing):
```
>> group target_ds 0.1
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

4. Load and normalize another dataset to use as queries. We can omit the argument for separator since space is set as default.
```
>> load query_ds ../datasets/test/test_10_20_space.txt
Dataset loaded
  Name:        ../datasets/test/test_10_20_space.txt
  ID:          1
  Item count:  10
  Item length: 20

>> normalize query_ds
Dataset query_ds is now normalized
```

We can list the datasets currently in memory by using `list dataset`
```
>> list dataset
2 datasets loaded

  query_ds              Normalized
  target_ds             Normalized      Grouped
```

5. Find top 10 similar time series from dataset `target_ds` to the time series 0[0, 10] in dataset `query_ds`
```
>> ksim 10 target_ds query_ds 0 0 0 100
Command executed in 0.00217s
Target dataset: target_ds
Query dataset: query_ds
k = 10. ke = 100
Query time series: 0 [-1, -1]
Timeseries 3 [1, 19] - distance = 0.2499
Timeseries 10 [0, 19] - distance = 0.2511
Timeseries 10 [0, 18] - distance = 0.2522
Timeseries 10 [0, 20] - distance = 0.2524
Timeseries 3 [1, 20] - distance = 0.2537
Timeseries 4 [1, 19] - distance = 0.254
Timeseries 7 [2, 20] - distance = 0.2548
Timeseries 10 [1, 19] - distance = 0.255
Timeseries 4 [1, 20] - distance = 0.2552
Timeseries 7 [1, 19] - distance = 0.2556
```

## Acknowledgement

The Genex codebase includes the trillionDTW [0] methods, uses the same pruning
methods, and even uses the trillion implementation of lemire lower/upper
bound.

## Reference

[0] http://www.cs.ucr.edu/~eamonn/SIGKDD_trillion.pdf

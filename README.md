# GENEX

GENEX (General Exploration System) is a time series exploration tool that facilities series exploration with multiple distances.

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
- Install dependencies:
```bash
sudo apt-get install libreadline6 libreadline6-dev python-dev zlibc zlib1g zlib1g-dev
```
- Install Boost 1.66 or above with Boost.Python:
```bash
wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
tar xzf boost_1_66_0.tar.gz
cd boost_1_66_0/
./bootstrap.sh --with-libraries=serialization,iostreams,python,test
./b2 install
```

### Windows (Untested):

- Install [CMake for Windows](https://cmake.org/download/). Make sure that the **bin** directory of CMake is in your PATH environment variable.
- Install [MinGW Distro with Boost](https://nuwen.net/mingw.html) in **C:\\**
- Install [WinEditLine](https://sourceforge.net/projects/mingweditline/files/) in **C:\\**
- Copy and replace **C:\wineditline\include\editline\readline.h** with **readline.h** in this repository.

## Build and Run

GENEX comes as a command line interface and a Python module. 

### Linux & macOS
Change current directory to this repository and run the following commands.
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

To install the Python module, activate your virtual environment, if any, then run the following command in the root folder of GENEX.
```bash
python setup.py install
```

After that, you can import the module `pygenex`.
```python
import pygenex as pg
```

Documentation of the available functions can be found in `pygenex/GENEXBindings.cpp`.

### Windows (Untested)

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

## Adding a new distance

To add a new distance, add a new class to the folder `genex/distance` with the template shown below and register the new distance. Instructions for how to register a new distance can be found in `Distance.cpp`.

```C++
#ifndef NEWDISTANCE_H
#define NEWDISTANCE_H

#include "TimeSeries.hpp"
#include "Exception.hpp"

namespace genex {

class NewDistance
{
public:
  IDT init() const {...}

  IDT reduce(IDT next, IDT prev, data_t x_1, data_t x_2) const {...}

  data_t norm(IDT total, const TimeSeries& t_1, const TimeSeries& t_2) const {...}

  data_t normDTW(IDT total, const TimeSeries& t_1, const TimeSeries& t_2) const {...}

  void clean(IDT x) {...}
};

} // namespace genex

#endif // NEWDISTANCE_H

```
In this template, `data_t` is a pre-defined data type in `TimeSeries.hpp` (right now it is `double`), and `IDT` can be any data type. For example, in the definition of `Sorensen.hpp` and `Cosine.hpp`, `IDT` is `data_t*`. `IDT` stands for "Intermediate Data Type", which is used in more complex distances that require keeping track of the intermediate values. For simple distances such as Euclidean or Manhattan, it can simply be `data_t`.

For each new distance class, two versions will be generated: a pairwise version and a warped version.

### Pairwise distance
The pairwise version takes in two sequences `X` and `Y` of the same length then computes the distance as follows:

```
pairwise = init()
for i = 0 to length(X)
  pairwise = reduce(pairwise, pairwise, X[i], Y[i])
return norm(pairwise, X, Y)
```

`init()` is used to initialize the first value of the distance. For example, it is `0` for Euclidean distance or `-INF` for Chebyshev distance. For more complex distance, it can initialize an instance of a data structure.

`reduce()`, in general, takes in the previous distance value and the values of the current points then combines them according to a specific distance definition. Note that it also has a `next` argument, which will be explained in the warped version.

`norm()` normalizes the distance value in the end.

### Warped distance

The warped version takes in two sequences `X` and `Y` that are possibly of different lengthts. It uses the same `init()` and `reduce()` as the pairwise distance, but uses `normDTW()` to normalizes the final result.

Unlike pairwise distance where the `next` argument and the `prev` argument will take the same value, in warped distance, they can be different. For simple distances such as Euclidean or Manhattan, this difference is not significant and we can ignore the `next` argument when defining `reduce()`. However, for complex distances where `IDT` is a data struct or an array, `next` is a new object or memory allocated by GENEX. Therefore, we don't need to allocate a new object or memory inside the definition of `reduce()` and write the new values into the `next` object instead. Additionally, the `clean()` method has to be defined as well to instruct GENEX how to deallocate the object or memory.


## Acknowledgement

GENEX was designed and implemented by Cuong Nguyen, Charles Lovering, Ramoza Ahsan, and Rodica Neamtu.

The Genex codebase includes the trillionDTW [0] methods, uses the same pruning
methods, and even uses the trillion implementation of lemire lower/upper
bound.

## Reference

[0] http://www.cs.ucr.edu/~eamonn/SIGKDD_trillion.pdf

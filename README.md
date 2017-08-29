# Genex

## Why?

This project allows you to use warped versions of any distance! This is a powerful capability and opens avenues for further research. Furthermore, the underlying structure enables warped distance of more complicated functions to still be useful in a large scale due to its ability to precompute relationships within a time series dataset. Furthermore, the process of adding a new distance and using it is fast and easy (described below).

## Install build dependencies

Brew:

```bash
  brew install cmake
  brew install boost
  brew install readline
```

Linux:

```bash
  sudo apt-get install cmake
  sudo apt-get install boost
  sudo apt-get install readline
```

## Building the system

Build the project. The executable genex will be in the build folder.
Tests will also be run as part of the build script.

```bash
  ./run.sh
```

## Adding new distances

### Mathematical derivation

As described in `to-be-linked`, define a non-decreasing distance in terms of a recursive function. As part of this process you need to define what is neccesary to normalize your result. Furthermore, you need to store the information necessary to compute your distance recursively. These ideas are discussed in detail in the paper and are necessary for moving forward. Concrete *code* examples are given in folder `src/distance/`, and theoretical examples are given here (link to extra).

### Creating a distance

* Navigate to the folder `src/distance/`
* Each distance must extend the interface (abstract class) `DistanceMetric.hpp`
* Duplicate a distance function header file to work with
  * rename class and header file and name the header guards
* If you are creating a complicated distance and you need to cache more than one numeric value, implement a cache as in our `Cosine.hpp` example. This can be done directly in your distance's header file.
* Implement the `init` function
  * This function determines the first value in the recursive process of calculating a distance
  * Its generally zero and so usually will not need to be changed
  * Note: remember to return a new cache, not just a number
* Implement the `reduce` function
  * This is the critical part of the process
  * As defined in the `DistanceMetric.hpp` interface, its inputs must be a `Cache c`, `data_t x`, `data_t y`. It must calculate the distance between `x` and `y` and then store whatever information necessary to compute the distance given `c` in a new cache and return it.
  * You can implement the pointwise distance separately from the reduce function, but its not necessary (as a helper function.) Some of our examples do so.
  * Note: remember return a new cache with the new calculated values
* Implement the `norm` function
  * There are two critical pieces to this function
    * (a) it normalizes distance as you wish it to (divide by n, 2, etc)
    * (b) Converts from a cache to a data_t
  * There is room to experiment here. As defined in the `DistanceMetric.hpp`, its inputs are references to both TimeSeries you are finding the distance between and the total distance so far saved into a Cache. A good starting point is computing the raw distance from Cache and dividing by the average of the lengths between the timeseries.
  * Note: return a data_t

### Add distance to list

* Add your class to the list of distances `gAllMetric` in the file `src/distance/Distance.cpp`
* Include the header file you created in this file as well.

### Finished

See notes on how to run the CLI below.

## CLI documentation

(currently see source for documented functions)

## Additional notes

We define a type data_t as a floating point number with precision that you can change dependent on your needs.

## Pull requests

We're exited to see others implement their own distance functions and add those distances to our (currently small) library of distances. To do this, please document your work using doxygen style and add tests to `test/distance`.

## Contact

All functions are documented, but if any issues come up please reach out to us at genex@wpi.edu.

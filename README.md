
# genex

### Why?
This project allows one to use warped versions of any distance! This is an  powerful capability and opens avenues for further research...

### Install build dependencies

Brew:
```
  brew install cmake
  brew install boost
  brew install readline
```

Linux:
```
  sudo apt-get install cmake
  sudo apt-get install boost
  sudo apt-get install readline
```

### Building the project
Build the project. The executable genex will be in the build folder.
Tests will also be run as part of the build script.

```
  ./run.sh
```

### Adding New Distances

#### Mathematical Derivation
As described in (link to paper), define a non-decreasing distance in terms
of a recursive function. Next consider what you wish/ you need to do to
normalize your result. Lastly, consider what you need to store the information
necessary to compute your distance recursively? These ideas are discussed
more deeply in the paper and are necessary for moving forward.

#### Duplicate a Template Distance file
* Navigate to the folder `src/distance/`
* Duplicate a distance function header file to work with
  * rename class and header file
* If you are creating a complicated distance and you need to cache more
than one numeric value, implement a cache as in our `Cosine.cpp` example.
* Implement the init function
  * Be sure to return a new cache with correct initization
* Implement the reduce function
  * Be sure to return a new cache with the new calculated values
* Implement the norm function (two critical pieces to this function)
  * (a) it normalizes distance as you wish it to (divide by n, 2, etc)
  * (b) Converts from a cache to a data_t

#### Add Distance to List
Added your class to the list of distance `gAllMetric` in the file

`src/distance/Distance.cpp`. Include the header file you created in
this folder as well.

#### Finished!
See notes on how to run the CLI below.

### CLI Documentation
(currently see source for documented functions)

### Additional notes
We define a type - data_t - that's precision is determined by a constant
to determine precision. All functions are documented, but please feel free to reach out to us if you think you find a mistake or have a question.

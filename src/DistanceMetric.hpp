#ifndef GENEX_SRC_DISTANCE_METRIC_H
#define GENEX_SRC_DISTANCE_METRIC_H

#include <iostream>

#include "TimeSeries.hpp"

// An abstract class that defines an API for distance metrics
// Example:
//    DistanceMetric * euc = new Euclidean;
//    cout << "The distance from A to B is " << euc->dist(A, B)
class DistanceMetric {
  public:
    // this function must be implemented as a monotonic distance function
    virtual data_t dist(data_t x_1, data_t x_2) = 0;

    // this function is the individual recursion of the distance for 
    // a single point of data
    // Example: SquaredEuclidean
    //  dist(points 1 through n for two timeseries) = (x_1_1 - x_2_1)^2 + ... + (x_1_n - x_2_n)^2
    //  dist(points 1 through n - 1) = a
    //  dist(points 1 through n - 1 and n) = a + (x_1_n - x_2_n)^2
    virtual data_t recurse(data_t a, data_t x_1, data_t x_2) = 0;

    // returns the normalization factor desired for this distance
    // Example: Euclidean
    //  returns t.getLength()
    // other options include 1, 2, sqrt(length)...
    virtual data_t norm(TimeSeries& t) = 0;
};

#endif // GENEX_SRC_DISTANCE_METRIC_H

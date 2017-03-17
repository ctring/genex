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
    virtual data_t dist(data_t x_1, data_t x_2) = 0;

    virtual data_t recurse(data_t a, data_t x_1, data_t x_2) = 0;

    virtual data_t norm(data_t p_1) = 0;
};

#endif // GENEX_SRC_DISTANCE_METRIC_H

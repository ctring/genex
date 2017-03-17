#ifndef GENEX_SRC_DISTANCE_METRIC_H
#define GENEX_SRC_DISTANCE_METRIC_H

#include <iostream>

#include "TimeSeries.hpp"

/** 
 *  @brief An abstract class that defines an API for distance metrics
 *
 *  Example:
 *    DistanceMetric * euc = new Euclidean;
 *    cout << "The distance from A to B is " << euc->dist(A, B)
 */
class DistanceMetric {
  public:
    /**
     *  @brief this function must be implemented as a monotonic distance function
     *
     *  This function returns the distance between to supplied points x_1 and x_2.
     *
     *  @param x_1 is data_t, a point (ie, x_j from time series x)
     *  @param x_2 is data_t, a point (ie, y_j from time series y)
     *  @return the distance between these two points using this metric
     */
    virtual data_t dist(data_t x_1, data_t x_2) = 0;

    /**
     *  @brief this function is the individual recursion of the distance for 
     *  
     *  This function is used as part of the general dynammic timewarping recursive
     *  step. For example:
     *
     *  dist(points 1 through n for two timeseries) = (x_1_1 - x_2_1)^2 + ... + (x_1_n - x_2_n)^2
     *  dist(points 1 through n - 1) = a
     *  dist(points 1 through n - 1 and n) = a + (x_1_n - x_2_n)^2
     *
     *  @param prev is the past total
     *  @param x_1 is a data point from one of the two timeseries
     *  @param x_2 is a data point from the other of the timeseries
     *  @return the distance between the previous and rest (defined recursively)
     */
     virtual data_t recurse(data_t a, data_t x_1, data_t x_2) = 0;

    /**
     *  @brief returns the normalized distance between two timeseries
     *
     *  Example: Euclidean
     *    (1 / N) * sqrt( sum( distances between timeseries )  )
     *  
     *  other options include 1, 2, sqrt(length)...
     *
     *  @param total is the total distance between the timeseries
     *  @param x_1 is one of the two TimeSeries between which distance is being found
     *  @param x_2 is the other of the two TimeSeries between which distance is being found
     *  @return the normalized distance between two time series 
     */
     virtual data_t norm(data_t total, TimeSeries& t_1, TimeSeries& t_2) = 0;
};

#endif // GENEX_SRC_DISTANCE_METRIC_H

#ifndef GENERAL_DISTANCE_H
#define GENERAL_DISTANCE_H

#include <vector>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"

namespace genex {
namespace distance {

/**
 *  @brief returns the an object representing a distance metric
 *
 *  @param metric_name name of a distance metric
 *  @return an object containing methods of the requested distance metric
 *  @throw GenexException if no distance with given name is found
 */
const DistanceMetric* getDistanceMetric(const std::string& metric_name);


/**
 *  @return a vector of names of available distances
 */
std::vector<std::string> getAllDistanceNames();

/**
 *  @brief returns the warped distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param a one of the two arrays of data
 *  @param b the other of the two arrays of data
 *  @param dropout drops the calculation of distance if within this
 */
data_t generalWarpedDistance(const DistanceMetric& metric,
                             const TimeSeries& x_1,
                             const TimeSeries& x_2,
                             data_t dropout);

/**
 *  @brief returns the distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param x_1 one of the time series
 *  @param x_2 the other of the time series
 *  @param x_3 the length of the data
 */
data_t generalDistance(const DistanceMetric& metric,
                       const TimeSeries& x_1,
                       const TimeSeries& x_2);

} // namespace distance
} // namespace genex

#endif //GENERAL_DISTANCE_H

#ifndef GENERAL_DISTANCE_H
#define GENERAL_DISTANCE_H

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"

namespace genex {
/**
 *  @brief returns the warped distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param x_1 one of the time series
 *  @param x_2 the other of the time series
 *  @param x_3 the length of the data
 */
data_t generalWarpedDistance(const DistanceMetric& metric, const TimeSeries& x_1, const TimeSeries& x_2, int len);

/**
 *  @brief returns the distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param x_1 one of the time series
 *  @param x_2 the other of the time series
 *  @param x_3 the length of the data
 */
data_t generalDistance(const DistanceMetric& metric, const TimeSeries& x_1, const TimeSeries& x_2, int len);

} // namespace genex

#endif //GENERAL_DISTANCE_H

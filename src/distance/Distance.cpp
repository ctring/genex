#include "distance/Distance.hpp"

namespace genex {

  /**
   *  @brief returns the warped distance between two sets of data
   *
   *  @param metric the distance metric to use
   *  @param x_1 one of the two arrays of data
   *  @param x_2 the other of the two arrays of data
   *  @param x_3 the length of the data
   */
  data_t generalWarpedDistance(const DistanceMetric& metric, const TimeSeries& x_1, const TimeSeries& x_2, int len)
  {
    //TODO implement this!!
    //TODO add dropout
    return generalDistance(metric, x_1, x_2, len);
  }

  /**
   *  @brief returns the distance between two sets of data
   *
   *  @param metric the distance metric to use
   *  @param x_1 one of the two arrays of data
   *  @param x_2 the other of the two arrays of data
   *  @param x_3 the length of the data
   */
  data_t generalDistance(const DistanceMetric& metric, const TimeSeries& x_1, const TimeSeries& x_2, int len)
  {
    data_t total = 0.0;
    for(int i = 0; i < len; i++) {
      total = metric.reduce(total, x_1[i], x_2[i]);
    }
    return metric.norm(total, x_1, x_2);
  }

} // namespace genex

#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <cmath>
#include <iostream>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"

namespace genex {

// This class is example of an implemented DistanceMetric
class Euclidean : public DistanceMetric
{
public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return pow(x_1 - x_2, 2);
  }

  data_t init() const
  {
    return 0;
  }

  data_t reduce(data_t prev, data_t x_1, data_t x_2) const
  {
    return prev + dist(x_1, x_2);
  }

  data_t norm(data_t total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    return sqrt(total) / t.getLength();
  }
};

} // namespace genex

#endif

#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <cmath>

#include "TimeSeries.hpp"
#include "Exception.hpp"

namespace genex {

// This class is example of an implemented DistanceMetric
class Euclidean
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

  data_t reduce(data_t next, data_t prev, data_t x_1, data_t x_2) const
  {
    return prev + dist(x_1, x_2);
  }

  data_t norm(data_t total, const TimeSeries& t_1, const TimeSeries& t_2) const
  {
    return sqrt((total) / (std::max(t_1.getLength(), t_2.getLength()) - 1));
  }

  data_t inverseNorm(data_t dropout, const TimeSeries& x_1, const TimeSeries& x_2) const
  {
    return dropout * dropout * (x_1.getLength() - 1);
  }

  void clean(data_t x) {}
};

} // namespace genex

#endif

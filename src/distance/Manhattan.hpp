#ifndef GENEX_SRC_MANHATTAN_H
#define GENEX_SRC_MANHATTAN_H

#include <iostream>
#include <cmath>

#include "distance/DistanceMetric.hpp"

namespace genex {

//This class is an example of an implemented DistanceMetric
class Manhattan : public DistanceMetric
{
public:
  data_t dist(data_t x_1, data_t x_2)
  {
    return std::abs(x_1 - x_2);
  };

  data_t recurse(data_t prev, const data_t x_1, const data_t x_2)
  {
    return prev + dist(x_1, x_2);
  };

  data_t norm(data_t total, const TimeSeries& t, const TimeSeries& t_2)
  {
    return total / t.getLength();
  };
};

} // namespace genex

#endif // GENEX_SRC_MINKOWSKI_H

#ifndef GENEX_SRC_MINKOWSKI_H
#define GENEX_SRC_MINKOWSKI_H

#include <iostream>
#include <cmath>
#include "DistanceMetric.hpp"


//This class is an example of an implemented DistanceMetric
class Minkowski : public DistanceMetric
{
public:
  data_t dist(const data_t x_1, const data_t x_2)
  {
    return std::abs(x_1 - x_2);
  };

  data_t recurse(const data_t prev, const data_t x_1, const data_t x_2)
  {
    return prev + dist(x_1, x_2);
  };

  data_t norm(const data_t total, const TimeSeries& t, const TimeSeries& t_2)
  {
    return total / t.getLength();
  };
};
#endif // GENEX_SRC_MINKOWSKI_H

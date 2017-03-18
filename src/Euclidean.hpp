#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <math.h>
#include <iostream>

#include "DistanceMetric.hpp"

// This class is example of an implemented DistanceMetric
class Euclidean : public DistanceMetric
{
public:
  data_t dist(data_t x_1, data_t x_2)
  {
    return pow(x_1 - x_2, 2);
  };

  data_t recurse(data_t prev, data_t x_1, data_t x_2)
  {
    return prev + dist(x_1, x_2);
  };

  data_t norm(data_t total, const TimeSeries& t, const TimeSeries& t_2)
  {
    return sqrt(total) / t.getLength();
  };
};

#endif

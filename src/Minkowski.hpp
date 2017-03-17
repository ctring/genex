#ifndef GENEX_SRC_MINKOWSKI_H
#define GENEX_SRC_MINKOWSKI_H

#include <iostream>
#include <cmath>
#include "DistanceMetric.hpp"

//This class is an example of an implemented DistanceMetric
class Minkowski : public DistanceMetric {
  public:
    data_t dist(data_t x_1, data_t x_2)
    {
      return std::abs(x_1 - x_2);
    };

    data_t recurse(data_t a, data_t x_1, data_t x_2)
    {
      return a + dist(x_1, x_2); 
    };

    data_t norm(data_t total, TimeSeries& t, TimeSeries& t_2)
    { 
      return total / t.getLength(); 
    };
};
#endif

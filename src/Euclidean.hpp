#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <iostream>
#include "DistanceMetric.hpp"

// This class is example of an implemented DistanceMetric
class Euclidean : public DistanceMetric {
  public:
    data_t dist(data_t x_1, data_t x_2)
    {
        data_t r = x_1 - x_2;
        return r * r;
    };

    data_t recurse(data_t a, data_t x_1, data_t x_2)
    {
       return a + dist(x_1, x_2); 
    };

    data_t norm(TimeSeries& t)
    { 
       return t.getLength();
    };
};

#endif

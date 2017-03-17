#ifndef GENEX_SRC_MINKOWSKI_H
#define GENEX_SRC_MINKOWSKI_H

#include "DistanceMetric.hpp"
#include <iostream>

typedef double data_t;

//This class is an example of an implemented DistanceMetric
class Minkowski : public DistanceMetric {
  public:
    data_t dist(data_t x_1, data_t x_2)
    {
        data_t r = (x_1 - x_2);
        return r >= 0 ? r : -r;;
    };

    data_t recurse(data_t a, data_t x_1, data_t x_2)
    {
       return a + dist(x_1, x_2); 
    };

    data_t norm(data_t p_1)
    { 
       //what does this function do?
       //does it do the normalization, or does it return the constant?
       //should we give it the ts? or length of the ts?
       return 5; 
    };
};

#endif

#ifndef GENEX_SRC_CHEBYSHEV_H
#define GENEX_SRC_CHEBYSHEV_H

#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

//This class is an example of an implemented DistanceMetric
class Chebyshev : public DistanceMetric
{

public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return std::abs(x_1 - x_2);
  }

  data_t* init() const
  {
    data_t* newData = new data_t;
    *newData = -INF;
    return newData;
  }

  data_t* reduce(data_t* next, data_t* prev, const data_t x_1, const data_t x_2) const
  {
    *next = std::max(*prev, dist(x_1, x_2));
    return next;
  }

  data_t norm(data_t* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    return *total;
  }

  std::string getName() const
  {
    return "chebyshev";
  }

  std::string getDescription() const
  {
    return "Description of Chebyshev distance";
  }
};

} // namespace genex

#endif // GENEX_SRC_MINKOWSKI_H

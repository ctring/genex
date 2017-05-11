#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <cmath>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

// This class is example of an implemented DistanceMetric
class Euclidean : public DistanceMetric
{
public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return pow(x_1 - x_2, 2);
  }

  data_t* init() const
  {
    data_t* newData = new data_t;
    *newData = 0;
    return newData;
  }

  data_t* reduce(data_t* next, data_t* prev, data_t x_1, data_t x_2) const
  {
    *next = *prev + dist(x_1, x_2);
    return next;
  }

  data_t norm(data_t* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    return sqrt((*total) / (std::max(t.getLength(), t_2.getLength()) - 1));
  }

  std::string getName() const
  {
    return "euclidean";
  }

  std::string getDescription() const
  {
    return "Description of Euclidean distance";
  }
};

} // namespace genex

#endif

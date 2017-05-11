#ifndef GENEX_SRC_COSINE_H
#define GENEX_SRC_COSINE_H

#include <iostream>
#include <cmath>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

//This class is an example of an implemented DistanceMetric
class Cosine : public DistanceMetric
{
public:
  data_t* init() const
  {
    data_t* newData = new data_t[3];
    newData[0] = newData[1] = newData[2] = 0;
    return newData;
  }

  data_t* reduce(data_t* next, data_t* prev, const data_t x_1, const data_t x_2) const
  {
    next[0] = prev[0] + pow(x_1, 2);
    next[1] = prev[1] + pow(x_2, 2);
    next[2] = prev[2] + x_1 * x_2;
    return next;
  }

  data_t norm(data_t* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    return total[2] / (sqrt(total[0] * total[1]));
  }

  std::string getName() const
  {
    return "cosine";
  }

  std::string getDescription() const
  {
    return "Description of Cosine distance";
  }
};

} // namespace genex

#endif // GENEX_SRC_COSINE_H

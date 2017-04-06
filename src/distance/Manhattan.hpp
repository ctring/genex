#ifndef GENEX_SRC_MANHATTAN_H
#define GENEX_SRC_MANHATTAN_H

#include <iostream>
#include <cmath>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

//This class is an example of an implemented DistanceMetric
class Manhattan : public DistanceMetric
{

public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return std::abs(x_1 - x_2);
  }

  Cache* init() const
  {
    return new DefaultCache(0);
  }

  Cache* reduce(Cache* prev, const data_t x_1, const data_t x_2, bool copy) const
  {
    DefaultCache* a = createInCache<DefaultCache>(prev);
    DefaultCache* new_a = createOutCache<DefaultCache>(a, copy);
    new_a->val = a->val + dist(x_1, x_2);
    return new_a;
  }

  data_t norm(Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    DefaultCache* c = createInCache<DefaultCache>(total);
    return (c->val / t.getLength());
  }

  std::string getName() const
  {
    return "manhattan";
  }

  std::string getDescription() const
  {
    return "Description of Manhattan distance";
  }
};

} // namespace genex

#endif // GENEX_SRC_MINKOWSKI_H

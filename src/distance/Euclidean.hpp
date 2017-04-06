#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <cmath>
#include <math.h>       /* sqrt */

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

  Cache* init() const
  {
    return new DefaultCache(0.0);
  }

  Cache* reduce(Cache* prev, data_t x_1, data_t x_2, bool copy) const
  {
    DefaultCache* a = createInCache<DefaultCache>(prev);
    DefaultCache* new_a = createOutCache<DefaultCache>(a, copy);
    new_a->val = sqrt(pow(a->val, 2) + dist(x_1, x_2));
    return new_a;
  }

  data_t norm(Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    DefaultCache* c = createInCache<DefaultCache>(total);
    return (c->val) / t.getLength();
  }

  std::string getName() const
  {
    return "euclidean";
  }

  std::string getDescription() const
  {
    return "Description of Euclidiean distance";
  }
};

} // namespace genex

#endif

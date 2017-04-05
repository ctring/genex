#ifndef GENEX_SRC_ALT_EUCLIDEAN_H
#define GENEX_SRC_ALT_EUCLIDEAN_H

#include <cmath>
#include <math.h>       /* sqrt */

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

// This class is example of an implemented DistanceMetric
class AltEuclidean : public DistanceMetric
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

  Cache* reduce(const Cache* prev, data_t x_1, data_t x_2) const
  {
    const DefaultCache* c = castCache<DefaultCache>(prev);
    return new DefaultCache(sqrt(pow(c->val, 2) + dist(x_1, x_2)));
  }

  data_t norm(const Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    const DefaultCache* c = castCache<DefaultCache>(total);
    return (c->val) / sqrt(std::max(t.getLength(), t_2.getLength()) - 1);
  }

  std::string getName() const
  {
    return "alt";
  }

  std::string getDescription() const
  {
    return "Description of Euclidiean distance";
  }
};

} // namespace genex

#endif

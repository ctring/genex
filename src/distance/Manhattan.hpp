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

  Cache* reduce(const Cache* prev, const data_t x_1, const data_t x_2) const
  {
    if(const DefaultCache* c = dynamic_cast<const DefaultCache*>(prev))
    {
      return new DefaultCache(c->val + dist(x_1, x_2));
    }
    throw GenexException("Invalid cache type.");
  }

  data_t norm(const Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    if(const DefaultCache* c = dynamic_cast<const DefaultCache*>(total))
    {
      return (c->val / t.getLength());
    }
    throw GenexException("Invalid cache type.");
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

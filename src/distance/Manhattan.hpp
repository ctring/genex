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

  class ManhattanCache : public Cache {
    public:
      data_t val = 0;
      ManhattanCache(data_t val) : val(val) {};
      bool lessThan(const Cache* other) const
      {
        if(const ManhattanCache* c = dynamic_cast<const ManhattanCache*>(other))
        {
          return val < c->val;
        }
        throw GenexException("Incorrect cache type.");
      }
  };

public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return std::abs(x_1 - x_2);
  }

  Cache* init() const
  {
    return new ManhattanCache(0);
  }

  Cache* reduce(const Cache* prev, const data_t x_1, const data_t x_2) const
  {
    if(const ManhattanCache* c = dynamic_cast<const ManhattanCache*>(prev))
    {
      return new ManhattanCache(c->val + dist(x_1, x_2));
    }
    throw GenexException("Invalid cache type.");
  }

  data_t norm(const Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    if(const ManhattanCache* c = dynamic_cast<const ManhattanCache*>(total))
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

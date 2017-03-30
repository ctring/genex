#ifndef GENEX_SRC_EUCLIDEAN_H
#define GENEX_SRC_EUCLIDEAN_H

#include <cmath>
#include <iostream>//debug
#include <math.h>       /* sqrt */

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

// This class is example of an implemented DistanceMetric
class Euclidean : public DistanceMetric
{
  class EuclideanCache : public Cache {
    public:
      data_t val = 0;
      EuclideanCache(data_t val) : val(val) {};
      bool lessThan(const Cache* other) const
      {
        if(const EuclideanCache* c = dynamic_cast<const EuclideanCache*>(other))
        {
          return val < c->val;
        }
        throw GenexException("Incorrect cache type");
      }
  };

public:
  data_t dist(data_t x_1, data_t x_2) const
  {
    return pow(x_1 - x_2, 2);
  }

  Cache* init() const
  {
    return new EuclideanCache(0.0);
  }

  Cache* reduce(const Cache* prev, data_t x_1, data_t x_2) const
  {
    if (const EuclideanCache* c = dynamic_cast<const EuclideanCache *>(prev))
    {
      return new EuclideanCache(sqrt(pow(c->val, 2) + dist(x_1, x_2)));
    }
    throw GenexException("Incorrect cache type.");
  }

  data_t norm(const Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    // const EuclideanCache* c = CAST_TO(EuclidieanCache, total)

    if (const EuclideanCache* c = dynamic_cast<const EuclideanCache *>(total))
    {
      std::cout << "length: " << t.getLength() << std::endl;
      std::cout << "res: " << (c->val) / t.getLength() << std::endl;
      std::cout << "val: " << (c->val) << std::endl;
      ///###
      return (c->val) / t.getLength();
    }
    throw GenexException("Incorrect cache type.");
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

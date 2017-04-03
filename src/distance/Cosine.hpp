#ifndef GENEX_SRC_COSINE_H
#define GENEX_SRC_COSINE_H

#include <iostream>
#include <cmath>
#include <math.h>       /* sqrt */

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Exception.hpp"

namespace genex {

//This class is an example of an implemented DistanceMetric
class Cosine : public DistanceMetric
{
  class CosineCache : public Cache {
    public:
      data_t sumSqX1 = 0;
      data_t sumSqX2 = 0;
      data_t sumX1X2 = 0;

      CosineCache(data_t sumSqX1, data_t sumSqX2, data_t sumX1X2) :
        sumSqX1(sumSqX1), sumSqX2(sumSqX2), sumX1X2(sumX1X2) {};

      bool lessThan(const Cache* other) const
      {
        if(const CosineCache* c = dynamic_cast<const CosineCache*>(other))
        {
          data_t me = sumX1X2 / sqrt(sumSqX1) * sqrt(sumSqX2);
          data_t you = c->sumX1X2 / (sqrt(c->sumSqX1) * sqrt(c->sumSqX2));
          return me < you;
        }
        throw GenexException("Incorrect cache type");
      }
  };

public:
  Cache* init() const
  {
    return new CosineCache(0,0,0);
  }

  Cache* reduce(const Cache* prev, const data_t x_1, const data_t x_2) const
  {
    if(const CosineCache* c = dynamic_cast<const CosineCache*>(prev))
    {
      return new CosineCache(c->sumSqX1 + pow(x_1, 2), 
                             c->sumSqX2 + pow(x_2, 2),
                             c->sumX1X2 + x_1 * x_2);
    }
    throw GenexException("Invalid cache type.");
  }

  data_t norm(const Cache* total, const TimeSeries& t, const TimeSeries& t_2) const
  {
    if(const CosineCache* c = dynamic_cast<const CosineCache*>(total))
    {
      return c->sumX1X2 / (sqrt(c->sumSqX1) * sqrt(c->sumSqX2));
    }
    throw GenexException("Invalid cache type.");
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

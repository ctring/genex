#ifndef GENEX_SRC_DISTANCE_METRIC_H
#define GENEX_SRC_DISTANCE_METRIC_H

#include <iostream>
#include "TimeSeries.hpp"
#include "Exception.hpp"

namespace genex {

class Cache
{
public:
  virtual bool lessThan(const Cache* other) const = 0;
  virtual ~Cache() {};
};

class DefaultCache : public Cache {
public:
  data_t val = 0;
  DefaultCache(data_t val) : val(val) {};
  bool lessThan(const Cache* other) const
  {
    if(const DefaultCache* c = dynamic_cast<const DefaultCache*>(other))
    {
      return val < c->val;
    }
    throw GenexException("Incorrect cache type");
  }
};

template<typename T>
const T* castCache(const Cache* cache)
{
  const T* casted = dynamic_cast<const T*>(cache);
  if (casted == nullptr) {
    throw GenexException("Invalid cache type");
  }
  return casted;
}

/**
 *  @brief this is an abstract class that defines an API for distance metrics
 *
 *  Example:
 *    DistanceMetric * euc = new Euclidean;
 *    cout << "The distance from A to B is " << euc->dist(A, B)
 */
class DistanceMetric
{
public:

  /**
   *  @brief this function returns the initial value of the reduce chain.
   *
   *  @return the inital value for reduce
   */
  virtual Cache* init() const = 0;

  /**
   *  @brief this function is the individual recursion of the distance for
   *
   *  This function is used as part of the general dynammic timewarping recursive
   *  step. For example:
   *
   *  dist(points 1 through n for two timeseries) = (x_1_1 - x_2_1)^2 + ... + (x_1_n - x_2_n)^2
   *  dist(points 1 through n - 1) = a
   *  dist(points 1 through n - 1 and n) = a + (x_1_n - x_2_n)^2
   *
   *  @param prev is the past total
   *  @param x_1 is a data point from one of the two timeseries
   *  @param x_2 is a data point from the other of the timeseries
   *  @return the distance between the previous and rest (defined recursively)
   */
  virtual Cache* reduce(const Cache* prev, data_t x_1, data_t x_2) const = 0;

  /**
   *  @brief returns the normalized distance between two timeseries
   *
   *  Example: Euclidean
   *    (1 / N) * sqrt( sum( distances between timeseries )  )
   *
   *  other options include 1, 2, sqrt(length)...
   *
   *  @param total is the total distance between the timeseries
   *  @param x_1 is one of the two TimeSeries between which distance is being found
   *  @param x_2 is the other of the two TimeSeries between which distance is being found
   *  @return the normalized total distance between two time series
   */
  virtual data_t norm(const Cache* total, const TimeSeries& t_1, const TimeSeries& t_2) const = 0;

  /**
   *  @return the name of the distance
   */
  virtual std::string getName() const = 0;

  /**
   *  @return the description of the distance
   */
  virtual std::string getDescription() const = 0;

};

} // namespace genex

#endif // GENEX_SRC_DISTANCE_METRIC_H

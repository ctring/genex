#ifndef TIMESERIESCENTROID_H
#define TIMESERIESCENTROID_H

#include <string>
#include "TimeSeries.hpp"

namespace genex {

/**
 *  @brief the centroid of a group of sequences
 *
 */
class GroupCentroid : public TimeSeries
{
public:

  /**
   *  @brief constructor for GroupCentroid
   *
   *  @param length length of the centroid's group
   */
  GroupCentroid(int length)
    : TimeSeries(length), count(0), cacheValid(true), cachedCentroid(length){};

  /**
   *  @brief gets the count of this group (# of members)
   *
   *  @return count of this group
   */
  int getCount(void) const { return this->count; }

  /**
   *  @brief adds a sequence to the centroid's sum of value
   *
   *  @param data the array to be added
   */
  void addTimeSeries(const TimeSeries& data);

  /**
   *  @brief refreshes the value of the centroid
   *
   *  This function refreshes the cached centroid if valid,
   *  else calculates it first.
   */
  TimeSeries getCentroid(void);

private:
  int count;
  bool cacheValid;
  TimeSeries cachedCentroid;
};

} // namespace genex

#endif // TIMESERIESCENTROID_H

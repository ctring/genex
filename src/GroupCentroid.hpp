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
    : TimeSeries(length), count(0), cacheValid(true)
    {
      data_t* temp = new data_t[length];
      memset(temp, 0, length * sizeof(data_t));

      this->cachedCentroid = temp;
    }

  /**
   *  @brief gets the length of this group
   *
   *  @return length of this group
   */
  int getLength(void) const { return this->length; }

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
  void addArray(const TimeSeries data);

  /**
   *  @brief refreshes the value of the centroid
   *
   *  This function refreshes the cached centroid if valid,
   *  else calculates it first.
   */
  void refreshCentroid(void);

private:
  int length, count;
  data_t* sum;

  bool cacheValid;
  data_t* cachedCentroid;
};

} // namespace genex

#endif // TIMESERIESCENTROID_H

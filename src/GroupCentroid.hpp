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
    : TimeSeries(length), count(0), cacheValid(true) {
      this->cachedAverages = new data_t[length];
      memset(this->cachedAverages, 0, length * sizeof(data_t));
    };

  /**
   *  @brief deconstructor for GroupCentroid
   */
  ~GroupCentroid()
  {
    delete cachedAverages;
    cachedAverages = NULL;
  }

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
   *  @brief refreshes the value of the centroid and gets value
   *
   *
   *  This function refreshes the cached centroid if valid,
   *  else calculates it first. It then returns the averaged value
   *  at that point.
   *
   *  @param idx the data location to get the value from
   *  @return the value at that index of the centroid
   */
  data_t& operator[](int idx);

  /**
   *  @brief gets the SUM value at this index (for testing purposes)
   *
   *  @param idx the data location to get the value from
   *  @return the value at that index of the centroid
   */
  const data_t& getSumValue(int idx) const;

private:
  int count;
  bool cacheValid;
  data_t* cachedAverages;
};

} // namespace genex

#endif // TIMESERIESCENTROID_H

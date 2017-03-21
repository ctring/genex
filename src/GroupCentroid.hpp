#ifndef TIMESERIESCENTROID_H
#define TIMESERIESCENTROID_H

#include <string>
#include "TimeSeries.hpp"

namespace genex {

/**
 *  @brief the centroid of a group of sequences
 *
 */
class GroupCentroid
{
public:

  /**
   *  @brief constructor for GroupCentroid
   *
   *  @param length length of the centroid's group
   */
  GroupCentroid(int length)
    : length(length), count(0), cacheValid(true) {

      // Prepare containers with correct size
      data_t* temp_1 = new data_t[length];
      memset(temp_1, 0, length * sizeof(data_t));

      data_t* temp_2 = new data_t[length];
      memset(temp_2, 0, length * sizeof(data_t));

      // Point data to the new container and update the size variable
      this->sum = temp_1;
      this->cachedCentroid = temp_2;
    };

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
  void addArray(const data_t *data);

  /**
   *  @brief gets the value of the centroid
   *
   *  This function returns the cached centroid if valid,
   *  else calculates it first.
   *
   *  @return the values of the centroid
   */
  data_t* &getCentroid(void);

private:
  int length, count;
  data_t* sum;

  bool cacheValid;
  data_t* cachedCentroid;
};

} // namespace genex

#endif // TIMESERIESCENTROID_H

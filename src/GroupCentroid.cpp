#include "GroupCentroid.hpp"


namespace genex {

/**
 *  @brief adds a time series to the centroid's sum of value
 *
 *  @param data the array to be added
 */
void GroupCentroid::addArray(const TimeSeries data)
{
  //future
  //this->sum = this->sum + data;

  for (int i = 0; i < this->length; i++) //sum.size()
  {
    sum[i] += data[i];
  }

  this->count++;
  this->cacheValid = false;
}

/**
 *  @brief refreshes the value of the centroid
 *
 *  This function refreshes the cached centroid if valid,
 *  else calculates it first.
 */
void GroupCentroid::refreshCentroid(void)
{
  if (this->cacheValid) {
    return;
  }

  //future
  //cachedCentroid = this->sum / this->count;

  for (int i = 0; i < this->length; i++) {
    cachedCentroid[i] = sum[i] / count;
  }

  cacheValid = true;
  return;
}

}// namespace genex

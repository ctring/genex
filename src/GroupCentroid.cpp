#include "GroupCentroid.hpp"


namespace genex {

/**
 *  @brief adds a sequence to the centroid's sum of value
 *
 *  @param data the array to be added
 */
void GroupCentroid::addArray(const data_t *data)
{
  for (int i = 0; i < this->length; i++) //sum.size()
  {
    sum[i] += data[i];
  }

  this->count++;
  this->cacheValid = false;
}

/**
 *  @brief gets the value of the centroid
 *
 *  This function returns the cached centroid if valid,
 *  else calculates it first.
 *
 *  @return the values of the centroid
 */
data_t* &GroupCentroid::getCentroid(void)
{
  if (this->cacheValid) {
    return this->cachedCentroid;
  }

  for (int i = 0; i < this->length; i++) {
    cachedCentroid[i] = sum[i] / count;
  }

  cacheValid = true;

  return cachedCentroid;
}

}// namespace genex

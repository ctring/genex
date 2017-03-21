#include "GroupCentroid.hpp"


namespace genex {

void GroupCentroid::addArray(const TimeSeries data)
{
  *(this) += (data);
  this->count++;
  this->cacheValid = false;
}

TimeSeries GroupCentroid::getCentroid(void)
{
  if (this->cacheValid) {
    return this->cachedCentroid;
  }

  for (int i = 0; i < this->getLength(); i++) {
     this->cachedCentroid[i] = this->data[i] / this->count;
  }

  cacheValid = true;
  return this->cachedCentroid;
}

}// namespace genex

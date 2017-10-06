#include "TimeSeries.hpp"
#include "Exception.hpp"

#include "lib/trillionDTW.h"

#include <algorithm>
#include <iostream> // debug

namespace genex {

int calculateWarpingBandSize(int length, double ratio)
{
  int bandSize = floor(length * ratio);
  return std::min(bandSize, length - 1);
}


TimeSeries::~TimeSeries()
{
  // if object allocated the data, delete it
  if (this->isOwnerOfData)
  {
    delete this->data;
    this->data = nullptr;
  }
}

data_t& TimeSeries::operator[](int idx) const
{
  if (idx < 0 || idx >= this->length) {
    throw GenexException("Data point index is out of range");
  }
  return this->data[start + idx];
}

TimeSeries& TimeSeries::operator+=(const TimeSeries& other)
{
  if (other.getLength() != this->length)
  {
    throw GenexException("Two time series must have the same length for addition");
  }
  for (int i = 0; i < this->length; i++)
  {
    data[start + i] += other[i];
  }
  keoghCacheValid = false;
  return *this;
}

const data_t* TimeSeries::getKeoghLower(double warpingBandRatio) const
{
  if (!keoghCacheValid || warpingBandRatio != cachedWarpingBandRatio) {
    this->generateKeoghLU(warpingBandRatio);
    cachedWarpingBandRatio = warpingBandRatio;
  }
  return keoghLower;
}

const data_t* TimeSeries::getKeoghUpper(double warpingBandRatio) const
{
  if (!keoghCacheValid || warpingBandRatio != cachedWarpingBandRatio) {
    this->generateKeoghLU(warpingBandRatio);
    cachedWarpingBandRatio = warpingBandRatio;
  }
  return keoghUpper;
}

void TimeSeries::generateKeoghLU(double warpingBandRatio) const
{
  delete[] keoghLower;
  delete[] keoghUpper;

  keoghLower = new data_t[this->length];
  keoghUpper = new data_t[this->length];

  int bandSize = calculateWarpingBandSize(this->length, warpingBandRatio);

  // Function provided by trillionDTW codebase. See README
  lower_upper_lemire(this->data, this->length, bandSize,
                     this->keoghLower, this->keoghUpper);

  keoghCacheValid = true;
}

} // namespace genex

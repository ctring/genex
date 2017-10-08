#include "TimeSeries.hpp"
#include "Exception.hpp"

#include "lib/trillionDTW.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

namespace genex {


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

const data_t* TimeSeries::getKeoghLower(int warpingBand) const
{
  if (!keoghCacheValid || warpingBand != cachedWarpingBand) {
    this->generateKeoghLU(warpingBand);
    cachedWarpingBand = warpingBand;
  }
  return keoghLower;
}

const data_t* TimeSeries::getKeoghUpper(int warpingBand) const
{
  if (!keoghCacheValid || warpingBand != cachedWarpingBand) {
    this->generateKeoghLU(warpingBand);
    cachedWarpingBand = warpingBand;
  }
  return keoghUpper;
}

void TimeSeries::generateKeoghLU(int warpingBand) const
{
  delete[] keoghLower;
  delete[] keoghUpper;

  keoghLower = new data_t[this->length];
  keoghUpper = new data_t[this->length];

  warpingBand = min(warpingBand, this->length - 1);

  // Function provided by trillionDTW codebase. See README
  lower_upper_lemire(this->data, this->length, warpingBand,
                     this->keoghLower, this->keoghUpper);

  keoghCacheValid = true;
}

void TimeSeries::printData(std::ostream &out) const
{
  for (int i = 0; i < length; i++) {
    out << std::setprecision(std::numeric_limits<data_t>::digits10 + 1)
        << data[start + i] << " ";
  }
}

} // namespace genex

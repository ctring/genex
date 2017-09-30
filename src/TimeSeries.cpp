#include "TimeSeries.hpp"
#include "Exception.hpp"

#include "lib/trillionDTW.h"

#include <cmath>
#include <iostream> // debug

namespace genex {

double TimeSeries::warpingBandRatio = 0.5;

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
    throw GenexException("Index is out of range");
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

const data_t* TimeSeries::getKeoghLower() {
  if (!keoghCacheValid) {
    this->generateKeoghLU();
  }
  return keoghLower;
}

const data_t* TimeSeries::getKeoghUpper() {
  if (!keoghCacheValid) {
    this->generateKeoghLU();
  }
  return keoghUpper;
}

void TimeSeries::generateKeoghLU()
{
  delete[] keoghLower;
  delete[] keoghUpper;

  keoghLower = new data_t[this->length];
  keoghUpper = new data_t[this->length];

  int clippedBandSize = static_cast<int>(
    round(warpingBandRatio / 2 * this->length));
  clippedBandSize = min(clippedBandSize, this->length - 1);

  // Function provided by trillionDTW codebase. See README
  lower_upper_lemire(this->data, this->length, clippedBandSize,
                     this->keoghLower, this->keoghUpper);

  keoghCacheValid = true;
}

} // namespace genex

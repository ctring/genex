#include "TimeSeries.hpp"
#include "Exception.hpp"

#include "lib/trillionDTW.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstring>

// EXPERIMENT
int extraTimeSeries = 0;

namespace genex {


TimeSeries& TimeSeries::operator=(const TimeSeries& other)
{
  if (isOwnerOfData) {
    delete[] this->data;
  }
  isOwnerOfData = other.isOwnerOfData;
  index = other.index;
  start = other.start;
  end = other.end;
  length = other.length;
  if (other.isOwnerOfData)
  {
    this->data = new data_t[length];
    memcpy(this->data, other.data, length * sizeof(data_t));
  }
  else {
    this->data = other.data;
  }
  return *this;
}

TimeSeries& TimeSeries::operator=(TimeSeries&& other)
{
  data = other.data;
  index = other.index;
  start = other.start;
  end = other.end;
  length = other.length;
  isOwnerOfData = other.isOwnerOfData;
  other.data = nullptr;
  return *this;
}

TimeSeries::~TimeSeries()
{
  // if object allocated the data, delete it
  if (this->isOwnerOfData)
  {
    delete this->data;
    this->data = nullptr;
  }
  delete[] keoghLower;
  keoghLower = nullptr;
  delete[] keoghUpper;
  keoghUpper = nullptr;
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
  keoghLower = nullptr;
  delete[] keoghUpper;
  keoghUpper = nullptr;

  keoghLower = new data_t[this->length];
  keoghUpper = new data_t[this->length];

  warpingBand = min(warpingBand, this->length - 1);

  // Function provided by trillionDTW codebase
  lower_upper_lemire(this->data + this->start, this->length, warpingBand,
                     this->keoghLower, this->keoghUpper);

  keoghCacheValid = true;
}

const data_t* TimeSeries::getData() const
{
  return this->data;
}

string TimeSeries::getIdentifierString() const
{
  std::ostringstream os;
  os << this->index << " [" << this->start << ", " << this->end << "]";
  return os.str();
}

void TimeSeries::printData(std::ostream &out) const
{
  for (int i = 0; i < length; i++) {
    out << std::setprecision(std::numeric_limits<data_t>::digits10 + 1)
        << data[start + i] << " ";
  }
}

} // namespace genex

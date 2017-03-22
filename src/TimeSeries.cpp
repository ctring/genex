#include "TimeSeries.hpp"
#include "Exception.hpp"
#include <iostream> // debug
namespace genex {

TimeSeries::~TimeSeries()
{
  // if object allocated the data, delete it
  if (this->isOwnerOfData)
  {
    delete this->data;
    this->data = NULL;
  }
}

const data_t& TimeSeries::operator[](int idx) const
{
  if (idx < 0 || idx >= this->length) {
    throw GenexException("Index is out of range");
  }
  return this->data[start + idx];
}

data_t& TimeSeries::operator[](int idx)
{
  return const_cast<data_t &>(static_cast<const TimeSeries &>(*this)[idx]);
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
  return *this;
}

} // namespace genex

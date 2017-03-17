#include "TimeSeries.hpp"
#include "Exception.hpp"

data_t TimeSeries::operator[](int index) const
{
  if (index < 0 || index >= this->length) {
    throw GenexException("Index out of range");
  }
  return this->data[index];
}
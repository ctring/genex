#include "TimeSeries.hpp"
#include "Exception.hpp"

data_t TimeSeries::operator[](int idx) const
{
  if (idx < 0 || idx >= this->length) {
    throw GenexException("Index is out of range");
  }
  return this->data[idx];
}

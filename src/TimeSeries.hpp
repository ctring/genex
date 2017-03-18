#ifndef TIMESERIES_H
#define TIMESERIES_H

#include "config.hpp"

#ifdef SINGLE_PRECISION
typedef float data_t;
#else
typedef double data_t;
#endif

class TimeSeries
{
public:
  TimeSeries(const data_t *data, int index, int start, int end)
    : data(data), index(index), start(start), end(end) {
      this->length = end - start;
    };

  data_t operator[](int idx) const;
  int getLength() const { return this->length; }


private:
  const data_t* data;
  int index;
  int start;
  int end;
  int length;
};

#endif // TIMESERIES_H

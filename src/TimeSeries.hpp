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
private:
  const data_t* data;
  int id;
  int start;
  int end;
  int length;
public:
  TimeSeries(const data_t *data, int id, int start, int end)
    : data(data), id(id), start(start), end(end) {
      this->length = end - start;
    };

  data_t operator[](int index) const;
  int getLength() const { return this->length; }


};

#endif

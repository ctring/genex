#ifndef GENEX_SRC_SORENSEN_H
#define GENEX_SRC_SORENSEN_H

#include <cmath>

#include "TimeSeries.hpp"
#include "Exception.hpp"

namespace genex {

class Sorensen
{
public:
  data_t* init() const
  {
    data_t* newData = new data_t[2];
    newData[0] = newData[1] = 0;
    return newData;
  }

  data_t* reduce(data_t* next, data_t* prev, const data_t x_1, const data_t x_2) const
  {
    next[0] = prev[0] + fabs(x_1 - x_2);
    next[1] = prev[1] + fabs(x_1 + x_2);
    return next;
  }

  data_t norm(data_t* total, const TimeSeries& t_1, const TimeSeries& t_2) const
  {
    return total[0] / total[1];
  }

  data_t normDTW(data_t* total, const TimeSeries& t_1, const TimeSeries& t_2) const
  {
    return norm(total, t_1, t_2);
  }

  void clean(data_t* x) {
    delete[] x;
  }

};

} // namespace genex

#endif // GENEX_SRC_SORENSEN_H

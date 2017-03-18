#ifndef TIMESERIES_H
#define TIMESERIES_H

#include "config.hpp"

#ifdef SINGLE_PRECISION
typedef float data_t;
#else
typedef double data_t;
#endif

/**
 *  @brief header of a time series
 *
 *  A TimeSeries does not holds the actual data of a time series. It only has a pointer
 *  pointing to a part of data in a TimeSeriesSet. It also contains the starting position
 *  and ending position of the time series so that it can represent a sub-sequence.
 *  To be exact, a TimeSeries is a half-closed interval [start, end) of a time series in
 *  a TimeSeriesSet.
 *
 */
class TimeSeries
{
public:

  /**
   *  @brief constructor for TimeSeries
   *
   *  @param data a pointer pointing to the actual data
   *  @param index index of this time series in a TimeSeriesSet
   *  @param start starting position of this time series
   *  @param end ending position of this time series
   */
  TimeSeries(const data_t *data, int index, int start, int end)
    : data(data), index(index), start(start), end(end) {
      this->length = end - start;
    };

  /**
   *  @brief accesses a value of this time series
   *
   *  @param idx index of the value in this time series
   *  @return accessed value
   */
  data_t operator[](int idx) const;

  /**
   *  @brief gets the length of this time series
   *
   *  @return length of this time series
   */
  int getLength() const { return this->length; }

private:
  const data_t* data;
  int index;
  int start;
  int end;
  int length;
};

#endif // TIMESERIES_H

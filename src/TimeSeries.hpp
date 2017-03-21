#ifndef TIMESERIES_H
#define TIMESERIES_H

#include "config.hpp"
#include <string>

#ifdef SINGLE_PRECISION
typedef float data_t;
#else
typedef double data_t;
#endif

namespace genex {

/**
 *  @brief header of a time series
 *
 *  A TimeSeries does not holds the actual data of a time series. It only has a pointer
 *  pointing to a part of data in a TimeSeriesSet. It also contains the starting position
 *  and ending position of the time series so that it can represent a sub-sequence.
 *  To be exact, a TimeSeries is a half-closed interval [start, end) of a time series in
 *  a TimeSeriesSet.
 *
 *  Outside the context of TimeSeriesSet. TimeSeries can be used as a general purpose container
 *  by providing a pointer to an array of data and its length.
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
  TimeSeries(data_t *data, int index, int start, int end)
    : data(data), index(index), start(start), end(end), isOwnerOfData(false) {
      this->length = end - start;
    };

  /**
   *  @brief constructor for TimeSeries
   *
   *  This constructor is use when only data and length is needed
   *  to specify the time series. Index is set to 0, starting position
   *  is set to 0 and ending position is the length of the time series
   *
   *  @param data a pointer pointing to the actual data
   *  @param length length of the time series
   */
  TimeSeries(data_t *data, int length)
    : data(data), index(0), start(0), end(length), length(length), isOwnerOfData(false) {}

  /**
   *  @brief constructor for TimeSeries
   *
   *  This constructor is use when only length is needed
   *  to specify the time series. Index is set to 0, starting position
   *  is set to 0 and ending position is the length of the time series
   *  Memory is then allocated for the data.
   *
   *  @param length length of the time series
   */
  TimeSeries(int length)
    : index(0), start(0), end(length), length(length), isOwnerOfData(true)
  {
    this->data = new data_t[length];
    memset(this->data, 0, length * sizeof(data_t));
  }

  /**
   *  @brief destructor
   */
  virtual ~TimeSeries();

  /**
   *  @brief get a value of this time series
   *
   *  @param idx index of the value in this time series
   *  @return the value at idx
   *
   *  @throw GenexException if the index is out of bound
   */
  const data_t& operator[](int idx) const;

  /**
   *  @brief get reference to a value of this time series
   *
   *  @param idx index of the value in this time series
   *  @return a reference to the value at idx
   *
   *  @throw GenexException if the index is out of bound
   */
  data_t& operator[](int idx);

  /**
   *  @brief accumulate data from other time series into this one
   *
   *  The data of the given time series is added to this time series
   *  point-wise. The other time series must have the same length as this
   *  one.
   *  @param other the other time series to be added to this one.
   *  @return the reference to this time series
   *
   *  @throw GenexException if the other time series has different length
   *         from this one
   */
  TimeSeries& operator+=(const TimeSeries& other);

  /**
   *  @brief gets the length of this time series
   *
   *  @return length of this time series
   */
  int getLength() const { return this->length; }

protected:
  data_t* data;

private:
  int isOwnerOfData;
  int index;
  int start;
  int end;
  int length;
};

} // namespace genex

#endif // TIMESERIES_H

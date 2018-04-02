#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>

#include <boost/serialization/serialization.hpp>

#define INF std::numeric_limits<data_t>::infinity()
#define EPS 1e-12

// EXPERIMENT
extern int extraTimeSeries;

namespace genex {

typedef double data_t;

int calculateWarpingBandSize(int length, double ratio);

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
  TimeSeries(data_t *data, int index, int start, int end);

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
  TimeSeries(data_t *data, int length);

  /**
   *  @brief constructor for TimeSeries
   *
   *  This constructor is use when only length is needed
   *  to specify the time series. Index is set to 0, starting position
   *  is set to 0 and ending position is the length of the time series
   *  Memory is allocated for the data.
   *
   *  @param length length of the time series
   */
  TimeSeries(int length);

  /**
   * @brief Copy constructor
   */
  TimeSeries(const TimeSeries& other);

  /**
   *  @brief Copy assignment and move assignment
   */
  TimeSeries& operator=(const TimeSeries& other);
  TimeSeries& operator=(TimeSeries&& other);

  /**
   *  @brief destructor
   */
  ~TimeSeries();

  /**
   *  @brief get a value of this time series
   *
   *  @param idx index of the value in this time series
   *  @return the value at idx
   *
   *  @throw GenexException if the index is out of bound
   */
  data_t& operator[](int idx) const;

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
   * @brief compare two time series
   */
  bool operator==(const TimeSeries& other) const;

  /**
   *  @brief gets the length of this time series
   *
   *  @return length of this time series
   */
  int getLength() const { return this->length; }

  /**
   *  @brief gets the index of the data
   *
   *  @return the index of the data from its timeseries
   */
  int getIndex() const { return this->index; }

  /**
   *  @brief gets the start of the data
   *
   *  @return the start of the data from its timeseries
   */
  int getStart() const { return this->start; }

  /**
   *  @brief gets the end of the data
   *
   *  @return the end of the data from its timeseries
   */
  int getEnd() const { return this->end; }

  /**
   *  @brief returns the average of the whole time series
   */
  data_t average();

  const data_t* getKeoghLower(int warpingBand) const;
  const data_t* getKeoghUpper(int warpingBand) const;

  const data_t* getData() const;
  std::string getIdentifierString() const;

  friend std::ostream& operator<<(std::ostream&, const TimeSeries&);

private:
  // ATTENTION: Always offset by 'start' when indexing 'data' (i.e. data[start + i])
  data_t* data = nullptr;
  bool isOwnerOfData;
  int index;
  int start;
  int end;
  int length;

  mutable bool keoghCacheValid = false;
  mutable data_t* keoghLower = nullptr;
  mutable data_t* keoghUpper = nullptr;
  mutable double cachedWarpingBand;

  std::ostream &printData(std::ostream &out) const;
  
  /**
   * @brief generates the upper and lower envelope used in Keogh lower bound calculation
   * @param bandSize size of the Sakoe-Chiba warpping band
   */
  void generateKeoghLU(int warpingBand) const;

  /*************************
   *  Start serialization
   *************************/
  friend class boost::serialization::access;

  template <class A>
  void serialize(A &ar, unsigned) {
    for (int i = 0; i < this->length; i++) {
      ar & data[this->start + i];
    }
  }
  /*************************
   *  End serialization
   *************************/
};

/* For printing */
inline std::ostream &operator<<(std::ostream &os, const TimeSeries &ts) { 
    return ts.printData(os);
}

/**
 *  @brief a struct pairing a dist with a time series
 *
 */
struct candidate_time_series_t
{
  TimeSeries data;
  data_t dist;

  bool operator<(const candidate_time_series_t& rhs) const 
  {
    if (dist == rhs.dist)
    {
      if (data.getIndex() == rhs.data.getIndex())
      {
        if (data.getStart() == rhs.data.getStart())
        {
          return data.getLength() < rhs.data.getLength();
        }
        return data.getStart() < rhs.data.getStart();
      }
      return data.getIndex() < rhs.data.getIndex();
    }
    return dist < rhs.dist;
  }
  candidate_time_series_t(const TimeSeries& data, data_t dist) : data(data), dist(dist) {};
  candidate_time_series_t() : data(0), dist(0) {}

  bool operator==(const candidate_time_series_t& rhs) const
  {
    return data == rhs.data && abs(dist - rhs.dist) < EPS;
  }
};

} // namespace genex

#endif // TIMESERIES_H

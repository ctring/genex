#ifndef TIMESERIESSET_H
#define TIMESERIESSET_H

#include <string>

#include "TimeSeries.hpp"

/**
 *  @brief a TimeSeriesSet object contains values and information of a dataset
 *
 *  Example:
 *    TimeSeriesSet dataset("example")
 *    dataset.load("data.txt", 20);
 */
class TimeSeriesSet
{
public:

  /**
   *  @brief default constructor for TimeSeriesSet
   *
   *  Create a TimeSeriestSet object with is an empty string for name
   */
  TimeSeriesSet()
    : name(""), data(NULL), itemLength(0), itemCount(0) {};

  /**
   *  @brief constructor for TimeSeriesSet
   *
   *  @param name name of the dataset
   */
  TimeSeriesSet(const std::string& name)
    : name(name), data(NULL), itemLength(0), itemCount(0) {};

  /**
   *  @brief destructor
   */
  virtual ~TimeSeriesSet();

  /**
   *  @brief loads data from a text file to this object
   *
   *  Values in the text file have to be arranged in a table format. The number of
   *  values (a.k.a columns) is inferred from the first line. All lines must have
   *  the same number of columns. If the number of lines exceeds maxNumRow, only
   *  maxNumRow lines are read and the rest is discarded. On the other hand, if
   *  maxNumRow is larger than or equal to the actual number of lines, all lines
   *  are read.
   *
   *  @param filePath path to a text file
   *  @param maxNumRow maximum number of rows to be read
   *  @param separator a string containings possible separator characters for values
   *         in a line. Default: " "
   *  @param startCol columns before startCol are discarded. Default: 0
   */
  void loadData(const std::string& filePath, int maxNumRow, const std::string& separator = " ", int startCol = 0);

  /**
   * @brief clears all data
   */
  void clearData();

  /**
   * @brief gets length of each time series in the dataset
   *
   * @return length of each time series
   */
  int getItemLength() const { return itemLength; }

  /**
   * @brief gets number of time series in the dataset
   *
   * @return number of time series
   */
  int getItemCount() const { return itemCount; }

  /**
   * @brief gets the name of the dataset
   *
   * @return name of the dataset
   */
  const std::string& getName() const { return name; }

  /**
   * @brief gets a whole time series
   *
   * @param index index of the time series in this dataset
   * @return time series in the time series with given index
   */
  TimeSeries getTimeSeries(int index) const;

  /**
   * @brief gets a sub-sequence of a time series
   *
   * @param index index of the time series in this dataset
   * @param start starting position of the time series
   * @param end   ending position of the time series
   * @return a sub-sequence of a time series in the dataset
   */
  TimeSeries getTimeSeries(int index, int start, int end) const;

private:

  /**
   * Reallocate data to fit size. If size is smaller than before, only the part
   * that fits the new size is kept. If size is larger, empty data is added.
   */
  void _resizeData(int size);

  data_t** data;
  int itemLength;
  int itemCount;
  const std::string name;

};

#endif // TIMESERIESSET_H
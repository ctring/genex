#ifndef TIMESERIESSET_H
#define TIMESERIESSET_H

#include <string>
#include <vector>

#include "TimeSeries.hpp"
#include "distance/Distance.hpp"

using std::string;
using std::vector;

namespace genex {

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
   *  Create a TimeSeriestSet object
   */
  TimeSeriesSet();

  /**
   *  @brief destructor
   */
  virtual ~TimeSeriesSet();

  /**
   *  @brief loads data from a text file to the memory
   *
   *  Values in the text file have to be arranged in a table format. The number of
   *  values (a.k.a columns) is inferred from the first line. All lines must have
   *  the same number of columns. If the number of lines exceeds maxNumRow, only
   *  maxNumRow lines are read and the rest is discarded. If maxNumRow is larger than 
   *  or equal to the actual number of lines, or maxNumRow is not positive, all lines 
   *  are read.
   *
   *  @param filePath path to a text file
   *  @param maxNumRow maximum number of rows to be read. If this value is not positive,
   *         all lines are read
   *  @param startCol columns before startCol, in 0-based index, are discarded.
   *  @param separator a string containing possible separator characters for values
   *         in a line
   *  @param hasNameCol whether the first column (starting from startCol) is the one
   *         with names for each time series
   *  @return an id number used to refer to the loaded dataset
   *
   *  @throw GenexException if cannot read from the given file
   */
  void loadData(const string& filePath
               , int maxNumRow
               , int startCol
               , const string& separator
               , bool hasNameCol = false);

  void saveData(const string& filePath, char separator) const;

  /**
   * @brief clears all data
   */
  void clearData();

  /**
   * @brief gets length of each time series in the dataset
   *
   * @return length of each time series
   */
  int getItemLength() const { return this->itemLength; }

  /**
   * @brief gets number of time series in the dataset
   *
   * @return number of time series
   */
  int getItemCount() const { return this->itemCount; }

  /**
   * @brief gets name of a time series
   *
   * @return name of a time series
   */
  string getTimeSeriesName(int i) const;

  /**
   * @brief gets the file path of the dataset
   *
   * @return file path of the dataset
   */
  const string& getFilePath() const { return this->filePath; }

  /**
   * @brief gets a sub-sequence of a time series
   *
   * @param index index of the time series in this dataset
   * @param start starting position of the time series
   * @param end   ending position of the time series. Set this and 'start' to
   *              a negative number to get the whole time series.
   * @return a sub-sequence of a time series in the dataset
   *
   * @throw GenexException if index, start or end is not in intended range
   */
  TimeSeries getTimeSeries(int index, int start = -1, int end = -1) const;

  /**
   *  @brief normalizes the datset
   *  Each value in the dataset is transformed by the following formula:
   *    d = (d - min) / (max - min)
   *  Where min and max are respectively the minimum and maximum values
   *  across the whole dataset.
   *
   * @return a pair (min, max) - the minimum and maximum value across
   *          the whole dataset before being normalized.
   */
  std::pair<data_t, data_t> normalize();

  /**
  *  @brief check if the dataset is normalized
  */
  bool isNormalized() { return normalized; }

  /**
   *  @brief check if data is loaded
   */
  bool isLoaded() { return this->data != nullptr; }

  /**
   * @brief Exhaustively searches through timeseries set for k similar time series.
   * 
   * @param query to search for
   * @param k number of time series to find
   * @param distanceName distance to use
   *  
   * @vector vector of candidates with exact distance from query.
   */
  vector<candidate_time_series_t> getKBestMatchesBruteForce(const TimeSeries& query
                                                            , int k
                                                            , string distanceName = "euclidean");
  
  
protected:
  data_t* data = nullptr;
  vector<string> names;
  int itemLength;
  int itemCount;

private:
  string filePath;
  bool normalized;
};

} // namespace genex

#endif // TIMESERIESSET_H

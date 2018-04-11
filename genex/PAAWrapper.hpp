#ifndef PAA_TIME_SERIES_SET_H
#define PAA_TIME_SERIES_SET_H

#include <vector>

#include "TimeSeriesSet.hpp"

using std::vector;

namespace genex {

/**
 *  @brief a PAAWrapper wraps TimeSeriesSet with PAA capability.
 */
class PAAWrapper
{
public:

  PAAWrapper(const TimeSeriesSet&);

  /**
   * @brief Generates a matrix containing PAA information
   * 
   * Each row of this matrix corresponds to a time series in the original dataset. The jth element in a row
   * is a PAA of the corresponding time series offset by j.
   * 
   * @param blockSize size of a block for PAA.
   */
  void generatePAAMatrix(int blockSize);

  /**
   * @brief Get the PAA version of a time series in the associated dataset
   * 
   * @param index index of the time series
   * @param start starting position
   * @param end ending position
   * @return the PAA version of the time series
   */
  TimeSeries getPAA(int index, int start, int end);

  /**
   * @brief Searches through timeseries set for k similar time series, using PAA to 
   *        reduce the dimension, thus producing approximated results.
   * 
   * @param query to search for
   * @param k number of time series to find
   * @param distanceName distance to use
   *  
   * @vector vector of candidates with exact distance from query.
   */
  std::vector<candidate_time_series_t> getKBestMatchesPAA(
    const TimeSeries& query, int k, const string& distanceName);

private:
  const TimeSeriesSet& dataset;
  int blockSize;
  vector< vector<TimeSeries> > paaMat;
};

} // namespace genex

#endif // PAA_TIME_SERIES_SET_H
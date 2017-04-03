#ifndef GROUPABLE_TIME_SERIES_SET_H
#define GROUPABLE_TIME_SERIES_SET_H

#include "TimeSeriesSet.hpp"
#include "GroupsEqualLengthSet.hpp"

namespace genex {

/**
 *  @brief a GroupableTimeSeriesSet object is a TimeSeriesSet with grouping
 *         functionalities
 */
class GroupableTimeSeriesSet : public TimeSeriesSet
{
public:

  /**
   *  @brief Destructor
   *
   */
  ~GroupableTimeSeriesSet();

  /**
   *  @brief groups the datset into similarity groups
   *
   *  @param metric the metric to use for comparing similarity (distance)
   *  @param threshold to use for determing the bound of similarity
   *
   *  @return the number of groups created
   */
  int groupAllLengths(const DistanceMetric* metric, data_t threshold);

  /**
    *  @brief deletes and clears the groups
    */
  void resetGrouping();

  /**
   * @brief Finds the best matching subsequence in the dataset
   *
   * @param the timeseries to find the match for
   * @param the metric to use for finding the distance
   *
   * @return a struct containing the closest TimeSeries and the distance between them
   * @throws exception if dataset is not grouped
   */
  candidate_time_series_t getBestMatch(const TimeSeries& other, const DistanceMetric* metric) const;

private:
  GroupsEqualLengthSet* groupsAllLengthSet = nullptr;
};

} // namespace genex

#endif // GROUPABLE_TIME_SERIES_SET_H
#ifndef GROUPS_EQUAL_LENGTH_SET_H
#define GROUPS_EQUAL_LENGTH_SET_H

#include <vector>

#include "GroupsEqualLength.hpp"
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

namespace genex {

/**
 *  The set of all groups of equal lengths for a dataset
 */
class GroupsEqualLengthSet
{
public:

  /**
   *  @brief The constructor for the GroupsEqualLengthSet
   *
   *  @param dataset the dataset to group
   *  @param threshold the threshold for similarity within a group
   */
  GroupsEqualLengthSet(const TimeSeriesSet& dataset) : dataset(dataset) {};

  /**
   *  @brief The deconstructor for the GroupsEqualLengthSet
   */
  ~GroupsEqualLengthSet(void)
  {
    reset();
  }

  /**
   *  @brief groups the dataset into groups of equal length
   *    using the metric to determine similarity
   *
   *  @param metric the metric used to group by
   *  @param threshold the threshold to be group with
   *  @return the number of groups it creates
   */
  int group(const std::string& distance_name, data_t threshold);

  /**
   *  @brief gets the most similar sequence in the dataset
   *
   *  @param query gets most similar sequence to the query
   *  @return the best match in the dataset
   */
  candidate_time_series_t getBestMatch(const TimeSeries& query);

  /**
   *  @brief clears the groups
   */
  void reset(void);

  /**
   *  @brief returns true if dataset is grouped
   */
  bool grouped(void) const;

  data_t getThreshold(void) const { return this->threshold; }

  // TimeSeriesGrouping *getGroup(int length);
  // TimeSeriesGrouping *getFullGroup();
private:
  data_t threshold;
  std::vector<GroupsEqualLength*> groupsEqualLength;
  const TimeSeriesSet& dataset;
  dist_t pairwiseDistance;
  dist_t warpedDistance;
};

} // namespace genex
#endif //GROUPS_EQUAL_LENGTH_SET_H

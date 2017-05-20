#ifndef GROUPS_EQUAL_LENGTH_H
#define GROUPS_EQUAL_LENGTH_H

#include <vector>

#include "TimeSeries.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

namespace genex {

typedef std::pair<const Group*, data_t> candidate_group_t;

class GroupsEqualLength
{
public:
  /**
   *  @brief constructor for GroupsEqualLength
   *
   *  this class contains all the groups of a given length
   *  for a TimeSeriesSet
   *
   *  @param dataset the dataset that the class creates groups for
   *  @param length the length of each time series in each group
   */
  GroupsEqualLength(const TimeSeriesSet& dataset, int length);

  /**
   * @brief deconstructor for GroupsEqualLength
   */
  ~GroupsEqualLength();

  /**
   *  @brief returns the number of groups inside this object
   *  @return the number of groups
   */
  int getNumberOfGroups(void) const;

  /**
   *  @brief generates all the groups for the timeseries of this length
   *
   *  @param metric the distance metric to use when computing the groups
   *  @param threshold the threshold to use when splitting into new groups
   *  @return number of generated groups
   */
  int generateGroups(const dist_t pairwiseDistance, data_t threshold);

  /**
   *  @brief gets the group closest to a query (measured from the centroid)
   *
   *  @param query the time series we're operating with
   *  @param metric the metric that determines the distance between ts
   *  @param dropout the dropout optimization param
   */
  candidate_group_t getBestGroup(const TimeSeries& query,
                                 const dist_t warpedDistance,
                                 data_t dropout) const;

  /**
   *  @return a group with given index
   */
  const Group* getGroup(int idx) const;

private:
  int length, subTimeSeriesCount;
  const TimeSeriesSet& dataset;
  std::vector<Group*> groups;
  std::vector<group_membership_t> memberMap;
};

} // namespace genex

#endif //GROUPS_EQUAL_LENGTH_H

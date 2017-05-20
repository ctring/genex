#ifndef GROUP_HPP
#define GROUP_HPP

#include "config.hpp"

#include "TimeSeriesSet.hpp"
#include "GroupCentroid.hpp"
#include "distance/Distance.hpp"

namespace genex {

/**
 *  In context of a group, a member is represented by the index of a whole time
 *  series in a dataset and the starting position. These two numbers make up the
 *  'coordinate' of a member
 */
typedef std::pair<int, int> member_coord_t;

/**
 *  @brief a struct pairing a dist with a time series
 *
 */
struct candidate_time_series_t
{
  TimeSeries data;
  data_t dist;

  candidate_time_series_t(const TimeSeries& data, data_t dist) : data(data), dist(dist) {};
};

/**
 *  @brief a structure, used for identifying membership of a sub-time-series and
 *         the sub-time-series right before it in a group
 */
struct group_membership_t
{
  member_coord_t prev;
  int groupIndex;

  group_membership_t() {}
  group_membership_t(int groupIndex, member_coord_t prev)
    : groupIndex(groupIndex), prev(prev) {}
};

/**
 *  @brief a group of similar time series
 *
 */
class Group
{
public:

  /**
   *  @brief constructor for Group
   *
   */
  Group(int groupIndex, int memberLength, const TimeSeriesSet& dataset,
    std::vector<std::vector<group_membership_t>>& memberMap) :
    groupIndex(groupIndex),
    memberLength(memberLength),
    dataset(dataset),
    memberMap(memberMap),
    centroid(memberLength),
    lastMemberCoord(std::make_pair(-1, -1)),
    count(0) {}

  /**
   *  @brief adds a member to the group
   *
   *  @param seq which sequence the member is from
   *  @param start where the member starts in the data
   */
  void addMember(int index, int start);

  /**
   *  @brief set the centroid of the group
   *
   *  @param seq which sequence the member is from
   *  @param start where the member starts in the data
   */
  void setCentroid(int index, int start);

  /**
   *  @brief gets the length of each sequence in the group
   *
   *  @return length of this group
   */
  int getMemberLength(void) const { return this->memberLength; }

  /**
   *  @brief gets the number of members in the group
   *
   *  @return count of this group
   */
  int getCount(void) const { return this->count;  }

  /**
   *  @brief returns the distance between the centroid and the data
   *
   *  @param data the data to be finding the distance to
   *  @param metric the distance metric to use
   *  @param dropout upper bound for early stopping
   *  @return the distance between the data and the centroid using the metric
   */
  data_t distanceFromCentroid(const TimeSeries& query, const dist_t distance, data_t dropout);

  /**
   * ...
   */
  data_t warpDistanceFromCentroid(const TimeSeries& query, const dist_t distance, data_t dropout);

  /**
   * ...
   */
  candidate_time_series_t getBestMatch(const TimeSeries& query, const dist_t distance) const;

  /**
   *  @brief gets the centroid of the group
   *
   *  @return values of the centroid
   */
  const GroupCentroid& getCentroid() const
  {
    return this->centroid;
  }

  //TODO
  //candidate_time_series_t getBestDistinctMatch(TimeSeriesIntervalEnvelope query, int warps=-1, double dropout=INF, int qSeq=-1);
  //vector<candidate_time_series_t> getSeasonal(int);
  //vector<TimeSeriesInterval> getGroupValues(void);

private:
  const TimeSeriesSet& dataset;
  std::vector<std::vector<group_membership_t>>& memberMap;

  int groupIndex;

  member_coord_t lastMemberCoord;

  int memberLength;
  int count;

  GroupCentroid centroid;
};

} // namespace genex
#endif //GROUP_HPP

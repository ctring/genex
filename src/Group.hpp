#ifndef GROUP_HPP
#define GROUP_HPP

#include "config.hpp"

#include "TimeSeries.hpp"   // INF
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"

#include <fstream>

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

  bool operator<(const candidate_time_series_t& rhs) const 
  {
      return dist < rhs.dist;
  }
  candidate_time_series_t(const TimeSeries& data, data_t dist) : data(data), dist(dist) {}
  candidate_time_series_t() : data(0), dist(0) {}
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
 *  @brief a structure, used for identifying a group 
 *
 *  {@link memberLength} and {@link index} allow this group to be identified.
 *  @members is used for tracking the correct number of groups
 *  @dist is the distance from a query to the group's representative.
 */
 struct group_index_t
 {
   int length;
   int index;
   int members;
   data_t dist;

   group_index_t(int length, int index, int members, data_t dist) 
      : length(length), index(index), members(members), 
          dist(dist) {};

   bool operator<(const group_index_t& rhs) const 
   {
       return dist < rhs.dist;
   }
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
  Group(int groupIndex, int memberLength, int subTimeSeriesCount, const TimeSeriesSet& dataset,
    std::vector<group_membership_t>& memberMap) :
    groupIndex(groupIndex),
    memberLength(memberLength),
    subTimeSeriesCount(subTimeSeriesCount),
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
   *  @param index index of sequence the centroid is from
   *  @param start where the centroid starts in the data
   */
  void setCentroid(int index, int start);

  /**
   *  @brief gets the centroid of the group
   *
   *  @return values of the centroid
   */
  const TimeSeries& getCentroid() const
  {
    return this->centroid;
  }

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
   *  @brief returns the distance between the centroid and the query
   *
   *  @param query the query to be finding the distance to
   *  @param pairwiseDistance the pairwise distance to use
   *  @param dropout upper bound for early stopping
   *  @return the distance between the query and the centroid
   */
  data_t distanceFromCentroid(const TimeSeries& query, const dist_t pairwiseDistance, data_t dropout);

  /**
   *  @brief gets the best match of a query in this group using the given distance
   */
  candidate_time_series_t getBestMatch(const TimeSeries& query, const dist_t distance) const;

  /**
   *  @brief gets all the members in a group
   *
   *  @return the TimeSeries for each value in the group.
   */
  const std::vector<TimeSeries> getMembers() const;

  /**
   *  @brief performs necessary KNN operations a group
   *
   *  @param query to find similar to
   *  @param k is the adjusted k, how many neighbors to find within the group.
   *  @param warpedDistance to be used for the distance metric
   *  @return neighbors
   */
  const std::vector<candidate_time_series_t> intraGroupKNN(
      const TimeSeries& query, int k, const dist_t warpedDistance) const;
  
  void saveGroup(std::ofstream &fout) const;
  void loadGroup(std::ifstream &fin);

private:
  const TimeSeriesSet& dataset;
  std::vector<group_membership_t>& memberMap;

  int groupIndex;

  member_coord_t lastMemberCoord;

  int memberLength;
  int subTimeSeriesCount;
  int count;

  TimeSeries centroid;
};

} // namespace genex
#endif //GROUP_HPP

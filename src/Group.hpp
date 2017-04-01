#ifndef GROUP_HPP
#define GROUP_HPP

#include "config.hpp"

#include "TimeSeriesSet.hpp"
#include "GroupCentroid.hpp"
#include "distance/DistanceMetric.hpp"

namespace genex {

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
 *  @brief a node structure, used for identifying membership of a
 *    timeseries and pointer to the next
 *
 */
struct node_t
{
  int index, start;
  node_t* next;

  node_t(int index, int start, node_t* next) : index(index), start(start), next(next) {};
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
   *  @param data a pointer the timeseries set that this is a group from
   *  @param length the length of the sequences in this set
   */
  Group(const TimeSeriesSet& dataset, int memberLength)
  : count(0), dataset(dataset), memberLength(memberLength), centroid(memberLength)
  {};

  ~Group();

  /**
   *  @brief adds a member to the group
   *
   *  @param seq which sequence the member is from
   *  @param start where the member starts in the data
   */
  void addMember(int index, int start);

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
  data_t distanceFromCentroid(const TimeSeries& query, const DistanceMetric* metric, data_t dropout);

  /**
   * ...
   */
  candidate_time_series_t getBestMatch(const TimeSeries& query, const DistanceMetric* metric) const;

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

  node_t* lastMember = nullptr;

  int memberLength;
  int count;

  GroupCentroid centroid;
};

} // namespace genex
#endif //GROUP_HPP

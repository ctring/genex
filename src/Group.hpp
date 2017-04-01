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
struct candidate_t
{
  TimeSeries* data;
  data_t dist;

  bool operator<(candidate_t &other)
  {
      return dist < other.dist;
  }

  candidate_t(data_t dist) : dist(dist) {};
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
  : count(0), dataset(dataset), memberLength(memberLength), centroid(memberLength), id(Group::next_id++)
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
   *  @param len the length of the data
   *  @param data the data to be finding the distance to
   *  @param metric the distance metric to use
   *  @return the distance between the data and the centroid using the metric
   */
  data_t distanceFromCentroid(const TimeSeries& query, const DistanceMetric* metric, data_t dropout);

  /**
   *  @brief gets the best match within the group
   *
   *  @param query finding the best match to the query in the group
   *  @param metric the distance metric
   *  @return values of the centroid
   */
  candidate_t getBestMatch(const TimeSeries& query, const DistanceMetric* metric, data_t dropout);

  /**
   *  @brief gets the centroid of the group
   *
   *  @return values of the centroid
   */
  TimeSeries& getCentroid(void)
  {
    return centroid;
  }

  int getId(void)
  {
    return id;
  }

  //TODO
  //candidate_t getBestDistinctMatch(TimeSeriesIntervalEnvelope query, int warps=-1, double dropout=INF, int qSeq=-1);
  //vector<candidate_t> getSeasonal(int);
  //vector<TimeSeriesInterval> getGroupValues(void);

private:
  static int next_id; // I think we don't need ids anymore...
  const int id;
  const TimeSeriesSet& dataset;
  //node_t* memberMap;
  node_t* lastMember = nullptr;

  int memberLength;
  int count;

  GroupCentroid centroid;
};

} // namespace genex
#endif //GROUP_HPP

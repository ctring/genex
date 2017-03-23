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
    int data;
    node_t* next;

    node_t() : data(-1), next(nullptr) {};
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
Group(const TimeSeriesSet& dataset, node_t* memberMap, int memberLength)
  : count(0), dataset(dataset), memberLength(memberLength), centroid(memberLength), memberMap(memberMap), id(Group::next_id++)
  {
    // number of indices required to represent all sequences
    this->repLength = dataset.getItemLength() - memberLength + 1;
  };

  /**
   *  @brief adds a member to the group
   *
   *  @param seq which sequence the member is from
   *  @param start where the member starts in the data
   */
  void addMember(int index, int start); //, bool update=true

  /**
   *  @brief returns if a member is part of the group
   *
   *  @param seq which sequence the member is from
   *  @param start where the member starts in the data
   *  @return whether or not the sequence is part of the group
   */
  bool isMember(int index, int start) const;

  /**
   *  @brief gets the length of each sequence in the group
   *
   *  @return length of this group
   */
  int getMemberLength(void) const { return this->memberLength; }

  /**
   *  @brief the length of each full time series
   *
   *  @return length of this group
   */
  int getRepLength(void) const { return this->repLength; }

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
  data_t distance(int len, const TimeSeries& query, const DistanceMetric& metric); //data_t dropout=INF

  /**
   *  @brief gets the best match within the group
   *
   *  @return values of the centroid
   */
  candidate_t getBestMatch(int len, const TimeSeries& query, const DistanceMetric& metric);

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
  static int next_id;
  const int id;
  const TimeSeriesSet& dataset;
  node_t* memberMap;
  node_t* lastMember = nullptr;

  int memberLength, repLength;
  int count;

  GroupCentroid centroid;
};

//each group will have a unique ID, not per equal length, but thats fine

} // namespace genex
#endif //GROUP_HPP

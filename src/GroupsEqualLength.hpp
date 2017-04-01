#ifndef GROUPS_EQUAL_LENGTH_H
#define GROUPS_EQUAL_LENGTH_H

#include <vector>

#include "TimeSeries.hpp"
#include "distance/DistanceMetric.hpp"
#include "Group.hpp"

namespace genex {

class GroupsEqualLength
{
public:
  /**
   *  @brief constructor for GroupsEqualLength
   *
   *  this class contains all the groups of a given length
   *  for a timeseriesset
   *
   *  @param dataset the dataset that the class creates groups for
   *  @param length the length of each group
   */
  GroupsEqualLength(const TimeSeriesSet& dataset, int length) :
    dataset(dataset), length(length)
    {
      this->perSeq = dataset.getItemLength() - length + 1;
      this->groups = std::vector<Group*>();
    }

  /**
   * @brief deconstructor for GroupsEqualLength
   */
  ~GroupsEqualLength();

  /**
   *  @brief gets the group at a given index
   *
   *  @param idx the index of the group to be returned
   *  @return the group
   */
  Group *getGroup(int idx) const;

  /**
   *  @brief returns the number of groups inside this object
   *  @return the number of groups
   */
  int getCount(void) const;

  /**
   *  @brief generates all the groups for the timeseries of this length
   *
   *  @param metric the distance metric to use when computing the groups
   *  @param threshold the threshold to use when splitting into new groups
   */
  void genGroups(DistanceMetric* metric, data_t threshold);

  /**
   *  @brief clears all the contents of all the groups in this object
   */
  void clearGroups(void);

  /**
   *  @brief gets the group closest to a query (measured from the centroid)
   *
   *  @param query the time series we're operating with
   *  @param metric the metric that determines the distance between ts
   *  @param dropout the dropout optimization param
   */
  int getBestGroup(const TimeSeries& query, DistanceMetric* metric, data_t& dist, data_t dropout=INF) const; //seqitem_t *dist=NULL, int warps=-1,

private:
  int length, perSeq;
  const TimeSeriesSet& dataset;
  std::vector<Group*> groups;
};

} // namespace genex

#endif //GROUPS_EQUAL_LENGTH_H

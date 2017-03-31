#ifndef GROUPSEQUALLENGTH_SET_H
#define GROUPSEQUALLENGTH_SET_H

#include <vector>

#include "GroupsEqualLength.hpp"
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/DistanceMetric.hpp"
#include "Group.hpp"

namespace genex {

class GroupsEqualLengthSet
{
public:
  GroupsEqualLengthSet(const TimeSeriesSet& dataset, data_t threshold=0.5) :
    dataset(dataset), threshold(threshold) {};

  ~GroupsEqualLengthSet(void)
  {
    reset();
  }

  int group(DistanceMetric* metric);

  candidate_t getBestMatch(const TimeSeries& data, DistanceMetric* metric);

  void reset(void);

  bool valid(void);

  data_t getThreshold(void)
  {
    return this->threshold;
  }

  void setThreshold(data_t threshold)
  {
    this->threshold = threshold;
  }

    // TimeSeriesGrouping *getGroup(int length);
    // TimeSeriesGrouping *getFullGroup();
private:
  data_t threshold;
  std::vector<GroupsEqualLength*> groupsEqualLength;
  const TimeSeriesSet& dataset;
};

}
#endif //GROUPSEQUALLENGTH_SET_H

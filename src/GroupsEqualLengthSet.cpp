#include "GroupsEqualLengthSet.hpp"
#include "GroupsEqualLength.hpp"

#include <vector>
#include <iostream> //debug
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/DistanceMetric.hpp"
#include "Group.hpp"

namespace genex {

int GroupsEqualLengthSet::group(DistanceMetric* metric, data_t threshold)
{
  reset();
  this->groupsEqualLength.resize(dataset.getItemLength(), NULL);

  int numberOfGroups = 0;

  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    this->groupsEqualLength[i] = new GroupsEqualLength(dataset, i);
    this->groupsEqualLength[i]->genGroups(metric, threshold);
    numberOfGroups += this->groupsEqualLength[i]->getNumberOfGroups();
  }
  this->threshold = threshold;
  return numberOfGroups;
}

candidate_time_series_t GroupsEqualLengthSet::getBestMatch(const TimeSeries& data, DistanceMetric* metric)
{
  data_t bestSoFarDist = INF;
  const Group* bestSoFarGroup = nullptr;

  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    // this looks through each group of a certain length finding the best of those groups
    candidate_group_t candidate = this->groupsEqualLength[i]->getBestGroup(data, metric, bestSoFarDist);

    std::cout << " post gg " << candidate.second << std::endl;

    if (candidate.second < bestSoFarDist)
    {
      bestSoFarGroup = candidate.first;
      bestSoFarDist = candidate.second;
    }
  }

  return bestSoFarGroup->getBestMatch(data, metric);
}

void GroupsEqualLengthSet::reset(void)
{
  for (unsigned int i = 0; i < this->groupsEqualLength.size(); i++) {
    delete this->groupsEqualLength[i];
  }

  this->groupsEqualLength.clear();
}

bool GroupsEqualLengthSet::grouped(void) const
{
  return groupsEqualLength.size() > 0;
}

} // namespace genex

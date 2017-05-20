#include "GroupsEqualLengthSet.hpp"
#include "GroupsEqualLength.hpp"

#include <vector>
#include <iostream> //debug
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

namespace genex {

int GroupsEqualLengthSet::group(const std::string& distance_name, data_t threshold)
{
  reset();

  this->pairwiseDistance = getDistance(distance_name);
  this->warpedDistance   = getDistance(distance_name + "_warp");
  this->threshold = threshold;
  this->groupsEqualLength.resize(dataset.getItemLength() + 1, NULL);

  int numberOfGroups = 0;

  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    this->groupsEqualLength[i] = new GroupsEqualLength(dataset, i);
    int noOfGenerated = this->groupsEqualLength[i]->generateGroups(this->pairwiseDistance, threshold);
    numberOfGroups += noOfGenerated;
  }
  return numberOfGroups;
}

candidate_time_series_t GroupsEqualLengthSet::getBestMatch(const TimeSeries& data)
{
  data_t bestSoFarDist = INF;
  const Group* bestSoFarGroup = nullptr;
  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    // this looks through each group of a certain length finding the best of those groups
    candidate_group_t candidate = this->groupsEqualLength[i]->getBestGroup(data, this->warpedDistance, bestSoFarDist);

    if (candidate.second < bestSoFarDist)
    {
      bestSoFarGroup = candidate.first;
      bestSoFarDist = candidate.second;
    }
  }

  return bestSoFarGroup->getBestMatch(data, this->warpedDistance);
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

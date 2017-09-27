#include "GroupsEqualLengthSet.hpp"
#include "GroupsEqualLength.hpp"
#include <sstream>
#include <functional>
#include <queue>
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
  this->warpedDistance   = getDistance(distance_name + DTW_SUFFIX);
  this->threshold        = threshold;
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

std::vector<TimeSeries> GroupsEqualLengthSet::kNN(const TimeSeries& data, int k)
{
  std::vector<TimeSeries> best;
  int kPrime = k;
  std::priority_queue<group_index_t, std::vector<group_index_t>, 
    pless<group_index_t>> bestSoFar;
  
  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    // this looks through each group of a certain length keeping top sum-k groups
    kPrime = this->groupsEqualLength[i]->
        interLevelKNN(data, this->warpedDistance, bestSoFar, kPrime);
  }

  // get best of the worst
  if (!bestSoFar.empty()) 
  {
    group_index_t g = bestSoFar.top();
    bestSoFar.pop();
    std::priority_queue<candidate_time_series_t, std::vector<candidate_time_series_t>, 
        pless<candidate_time_series_t>> intraResults = 
            this->groupsEqualLength[g.length]->
                getGroup(g.index)->intraGroupKNN(data, kPrime+g.members, this->warpedDistance);
    // add all of the worst's best to answer
    for (; !intraResults.empty(); intraResults.pop()) 
    {
      best.push_back(intraResults.top().data);
    }
  }
          
  // add rest to the answer  
  for (; !bestSoFar.empty(); bestSoFar.pop()) // TODO: this doesn't *need* to be in order.
  {
    group_index_t g = bestSoFar.top();    
    std::vector<TimeSeries> allTimeSeriesInGroup = 
        this->groupsEqualLength[g.length]->getGroup(g.index)->getMembers();
    // add all of the worst's best to answer
    best.insert(std::end(best), std::begin(allTimeSeriesInGroup), std::end(allTimeSeriesInGroup));  
  }

  // clean up
  return best;
}

} // namespace genex

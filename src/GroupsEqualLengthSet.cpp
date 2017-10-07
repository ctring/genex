#include "GroupsEqualLengthSet.hpp"
#include "GroupsEqualLength.hpp"
#include <cmath>
#include <sstream>
#include <functional>
#include <queue>
#include <vector>
#include <iostream> //debug
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

using std::vector;
using std::max;
using std::min;
using std::cout;
using std::endl;

namespace genex {

int GroupsEqualLengthSet::group(const std::string& distance_name, data_t threshold)
{
  reset();

  this->pairwiseDistance = getDistance(distance_name);
  if (distance_name == "euclidean") {
    this->warpedDistance = cascadeDistance;
  }
  else {
    this->warpedDistance = getDistance(distance_name + DTW_SUFFIX);
  }
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

candidate_time_series_t GroupsEqualLengthSet::getBestMatch(const TimeSeries& query)
{
  if (query.getLength() <= 1) {
    throw GenexException("Length of query must be larger than 1");
  }
  data_t bestSoFarDist = INF;
  const Group* bestSoFarGroup = nullptr;

  vector<int> order (generateTraverseOrder(query.getLength(), this->groupsEqualLength.size() - 1));
  for (unsigned int io = 0; io < order.size(); io++) {
    int i = order[io];
    // this looks through each group of a certain length finding the best of those groups
    candidate_group_t candidate = this->groupsEqualLength[i]->getBestGroup(query, this->warpedDistance, bestSoFarDist);
    if (candidate.second < bestSoFarDist)
    {
      bestSoFarGroup = candidate.first;
      bestSoFarDist = candidate.second;
    }
  }
  return bestSoFarGroup->getBestMatch(query, this->warpedDistance);
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

vector<TimeSeries> GroupsEqualLengthSet::kNN(const TimeSeries& data, int k)
{
  vector<TimeSeries> best;
  vector<group_index_t> bestSoFar;
  int kPrime = k;
  
  // process each group of a certain length keeping top sum-k groups
  for (unsigned int i = 2; i < this->groupsEqualLength.size(); i++)
  {
    kPrime = this->groupsEqualLength[i]->
        interLevelKNN(data, this->warpedDistance, &bestSoFar, kPrime);
  }
  
  // process top group directly
  if (!bestSoFar.empty()) 
  {
    group_index_t g = bestSoFar.front();
    bestSoFar.erase(bestSoFar.begin());
    vector<candidate_time_series_t> intraResults = 
        this->groupsEqualLength[g.length]->
            getGroup(g.index)->intraGroupKNN(data, kPrime+g.members, this->warpedDistance);
    // add all of the worst's best to answer
    for (int i = 0; i < intraResults.size(); ++i) 
    {
      best.push_back(intraResults[i].data);
    }
  }
  
  // add all timeseries in the *better* groups 
  for (int i = 0; i < bestSoFar.size(); i++)
  {
    group_index_t g = bestSoFar[i];  
    vector<TimeSeries> members = 
        this->groupsEqualLength[g.length]->getGroup(g.index)->getMembers();
    best.insert(std::end(best), std::begin(members), std::end(members));  
  }

  // clean up
  return best;
}

vector<int> generateTraverseOrder(int queryLength, int totalLength)
{
  vector<int> order;
  int low = queryLength - 1;
  int high = queryLength + 1;
  bool lowStop = false, highStop = false;

  order.push_back(queryLength);
  while (!(lowStop && highStop)) {
    if (low < 2) lowStop = true;
    if (high > totalLength) highStop = true;

    if (!lowStop) {
      // queryLength is always larger than low
      int r = calculateWarpingBandSize(queryLength);
      if (low + r >= queryLength) {
        order.push_back(low);
        low--;
      }
      else {
        lowStop = true;
      }
    }

    if (!highStop) {
      // queryLength is always smaller than high
      int r = calculateWarpingBandSize(high);
      if (queryLength + r >= high) {
        order.push_back(high);
        high++;
      }
      else {
        highStop = true;
      }
    }
  }
  return order;
}

} // namespace genex

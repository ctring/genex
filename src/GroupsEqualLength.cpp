#include "GroupsEqualLength.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream> //debug

#include "TimeSeries.hpp"
#include "Group.hpp"
#include "Exception.hpp"
#include "distance/Distance.hpp"

namespace genex {

GroupsEqualLength::GroupsEqualLength(const TimeSeriesSet& dataset, int length)
 : dataset(dataset), length(length)
{
  this->subTimeSeriesCount = dataset.getItemLength() - length + 1;
  this->memberMap = std::vector<group_membership_t>(dataset.getItemCount() * this->subTimeSeriesCount);
}

GroupsEqualLength::~GroupsEqualLength()
{
  for (unsigned int i = 0; i < groups.size(); i++)
  {
    delete groups[i];
    groups[i] = NULL;
  }
  groups.clear();
}

const Group* GroupsEqualLength::getGroup(int idx) const
{
  if (idx < 0 || idx >= this->getNumberOfGroups()) {
    throw GenexException("Group index is out of range");
  }
  return this->groups[idx];
}

int GroupsEqualLength::getNumberOfGroups(void) const
{
  return this->groups.size();
}

int GroupsEqualLength::generateGroups(const dist_t pairwiseDistance, data_t threshold)
{
  for (int start = 0; start < this->subTimeSeriesCount; start++)
  {
    for (int idx = 0; idx < dataset.getItemCount(); idx++)
    {
      TimeSeries query = dataset.getTimeSeries(idx, start, start + this->length);

      data_t bestSoFar = INF;
      int bestSoFarIndex;

      for (unsigned int i = 0; i < groups.size(); i++)
      {
        data_t dist = this->groups[i]->distanceFromCentroid(query, pairwiseDistance, bestSoFar);
        if (dist < bestSoFar)
        {
          bestSoFar = dist;
          bestSoFarIndex = i;
        }
      }

      if (bestSoFar > threshold / 2 || this->groups.size() == 0)
      {
        bestSoFarIndex = this->groups.size();
        int newGroupIndex = this->groups.size();
        this->groups.push_back(new Group(newGroupIndex, this->length, this->subTimeSeriesCount,
                                         this->dataset, this->memberMap));
        this->groups[bestSoFarIndex]->setCentroid(idx, start);
      }

      this->groups[bestSoFarIndex]->addMember(idx, start);
    }
  }

  return this->getNumberOfGroups();
}

candidate_group_t GroupsEqualLength::getBestGroup(const TimeSeries& query,
                                                  const dist_t warpedDistance,
                                                  data_t dropout) const
{
  data_t bestSoFarDist = dropout;
  const Group* bestSoFarGroup = nullptr;
  for (unsigned int i = 0; i < groups.size(); i++) {

    data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, bestSoFarDist);
    if (dist < bestSoFarDist) {
      bestSoFarDist = dist;
      bestSoFarGroup = groups[i];
    }
  }

  return std::make_pair(bestSoFarGroup, bestSoFarDist);
}

int GroupsEqualLength::interLevelKNN(const TimeSeries& query, 
    const dist_t warpedDistance, 
    std::vector<group_index_t>* bestSoFar,
    int k)
{
  for (unsigned int i = 0; i < groups.size(); i++) {
      if (k <= 0) // if heap is full, keep only sum-k groups
      {
        data_t bestSoFarDist = bestSoFar->front().dist;
        data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, bestSoFarDist);
        if (dist < bestSoFarDist) {
          int membersAdded = groups[i]->getCount();
          bestSoFar->push_back(group_index_t(this->length, i, membersAdded, dist));
          std::push_heap(bestSoFar->begin(), bestSoFar->end());
          k -= membersAdded;
          // If the worst (furthest) group can be removed, with keeping at least k elements
          // tracked, remove it.
          while(k + bestSoFar->front().members <= 0) { 
            k += bestSoFar->front().members;
            std::pop_heap(bestSoFar->begin(), bestSoFar->end());
            bestSoFar->pop_back();
          }
        }
      }
      else // heap is not full, directly add to heap.
      {
        int membersAdded = groups[i]->getCount();        
        data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, INF);
        bestSoFar->push_back(group_index_t(this->length, i, membersAdded, dist));
        k -= membersAdded;
        if (k <= 0) {
          // heapify the heap exactly once when it becomes full.
          std::make_heap(bestSoFar->begin(), bestSoFar->end());          
        }
      }
  }
  return k;
}


} // namespace genex

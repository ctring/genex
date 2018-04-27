#include "group/LocalLengthGroupSpace.hpp"

#include <vector>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <iostream>
#include <chrono>

#include "TimeSeries.hpp"
#include "group/Group.hpp"
#include "Exception.hpp"
#include "distance/Distance.hpp"

using std::cout;
using std::ofstream;
using std::ifstream;
using std::endl;
using namespace std::chrono;

#define LOG_EVERY_S 10
#define LOG_FREQ  5

namespace genex {

LocalLengthGroupSpace::LocalLengthGroupSpace(const TimeSeriesSet& dataset, int length)
 : dataset(dataset), length(length)
{
  this->subTimeSeriesCount = dataset.getItemLength() - length + 1;
  this->memberMap = std::vector<group_membership_t>(dataset.getItemCount() * this->subTimeSeriesCount);
}

LocalLengthGroupSpace::~LocalLengthGroupSpace()
{
  reset();
}

void LocalLengthGroupSpace::reset()
{
  for (auto i = 0; i < groups.size(); i++)
  {
    delete groups[i];
    groups[i] = nullptr;
  }
  groups.clear();
}

std::atomic<long> gLastTime(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());

int LocalLengthGroupSpace::generateGroups(const dist_t pairwiseDistance, data_t threshold)
{
  long nowInSec = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
  long elapsedSeconds = nowInSec - gLastTime;
  bool doLog = false;
  if (elapsedSeconds >= LOG_EVERY_S) {
    doLog = true;
    gLastTime = nowInSec;
  }
  if (doLog) {
    cout << "Processing time series space of length " << this->length << endl;
  }
  int totalTimeSeries = this->subTimeSeriesCount * dataset.getItemCount();
  int counter = 0;
  for (int start = 0; start < this->subTimeSeriesCount; start++)
  {
    for (int idx = 0; idx < dataset.getItemCount(); idx++)
    {
      counter++;
      if (doLog) {
        if (counter % (totalTimeSeries / LOG_FREQ) == 0) {
          cout << "  Grouping progress... " << counter << "/" << totalTimeSeries 
               << " (" << counter*100/totalTimeSeries << "%)" << endl;
        }
      }

      TimeSeries query = dataset.getTimeSeries(idx, start, start + this->length);

      data_t bestSoFar = INF;
      int bestSoFarIndex;

      for (auto i = 0; i < groups.size(); i++)
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
        this->groups.push_back(new Group(newGroupIndex
                                         , this->length
                                         , this->subTimeSeriesCount
                                         , this->dataset
                                         , this->memberMap));
        this->groups[bestSoFarIndex]->setCentroid(idx, start);
      }

      this->groups[bestSoFarIndex]->addMember(idx, start);
    }
  }

  return this->getNumberOfGroups();
}

int LocalLengthGroupSpace::getNumberOfGroups(void) const
{
  return this->groups.size();
}

const Group* LocalLengthGroupSpace::getGroup(int idx) const
{
  if (idx < 0 || idx >= this->getNumberOfGroups()) {
    throw GenexException("Group index is out of range");
  }
  return this->groups[idx];
}

void LocalLengthGroupSpace::saveGroupsOld(ofstream &fout, bool groupSizeOnly) const 
{
  // Number of groups having time series of this length
  fout << groups.size() << endl;
  if (groupSizeOnly) {
    for (auto i = 0; i < groups.size(); i++) 
    {
      fout << groups[i]->getCount() << " ";
    }
    fout << endl;
  }
  else 
  {
    for (auto i = 0; i < groups.size(); i++) 
    {
      groups[i]->saveGroupOld(fout);
    }
  }
}

int LocalLengthGroupSpace::loadGroupsOld(ifstream &fin)
{
  reset();
  int numberOfGroups;
  fin >> numberOfGroups;
  for (auto i = 0; i < numberOfGroups; i++)
  {
    auto grp = new Group(i
                         , this->length
                         , this->subTimeSeriesCount
                         , this->dataset
                         , this->memberMap);
    grp->loadGroupOld(fin);
    this->groups.push_back(grp);
  }
  return numberOfGroups;
}

candidate_group_t LocalLengthGroupSpace::getBestGroup(const TimeSeries& query,
  const dist_t warpedDistance,
  data_t dropout) const
{
  data_t bestSoFarDist = dropout;
  const Group* bestSoFarGroup = nullptr;
  for (auto i = 0; i < groups.size(); i++) {
    data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, bestSoFarDist);
    if (dist < bestSoFarDist) {
      bestSoFarDist = dist;
      bestSoFarGroup = groups[i];
    }
  }

  return std::make_pair(bestSoFarGroup, bestSoFarDist);
}

int LocalLengthGroupSpace::interLevelKSim(const TimeSeries& query, 
    const dist_t warpedDistance,
    std::vector<group_index_t> &bestSoFar,
    int k)
{
  for (auto i = 0; i < groups.size(); i++) {
      if (k <= 0) // if heap is full, keep only sum-k groups
      {
        data_t bestSoFarDist = bestSoFar.front().dist;
        data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, bestSoFarDist);
        if (dist < bestSoFarDist) {
          int membersAdded = groups[i]->getCount();
          bestSoFar.push_back(group_index_t(this->length, i, membersAdded, dist));
          std::push_heap(bestSoFar.begin(), bestSoFar.end());
          k -= membersAdded;
          // If the worst (furthest) group can be removed, with keeping at least k elements
          // tracked, remove it.
          while(k + bestSoFar.front().members <= 0) { 
            k += bestSoFar.front().members;
            std::pop_heap(bestSoFar.begin(), bestSoFar.end());
            bestSoFar.pop_back();
          }
        }
      }
      else // heap is not full, directly add to heap.
      {
        int membersAdded = groups[i]->getCount();
        data_t dist = groups[i]->distanceFromCentroid(query, warpedDistance, INF);
        bestSoFar.push_back(group_index_t(this->length, i, membersAdded, dist));
        k -= membersAdded;
        if (k <= 0) {
          // heapify the heap exactly once when it becomes full.
          std::make_heap(bestSoFar.begin(), bestSoFar.end());
          while(k + bestSoFar.front().members <= 0) { 
            k += bestSoFar.front().members;
            std::pop_heap(bestSoFar.begin(), bestSoFar.end());
            bestSoFar.pop_back();
          }
        }
      }
  }
  return k;
}

} // namespace genex

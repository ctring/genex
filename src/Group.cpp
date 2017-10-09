#include "Group.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "TimeSeries.hpp"
#include "distance/Distance.hpp"

using std::vector;
using std::ofstream;
using std::ifstream;
using std::endl;

namespace genex {

void Group::addMember(int tsIndex, int tsStart)
{
  this->count++;
  this->memberMap[tsIndex * this->subTimeSeriesCount + tsStart] =
    group_membership_t(this->groupIndex, this->lastMemberCoord);
  this->lastMemberCoord = std::make_pair(tsIndex, tsStart);
}

void Group::setCentroid(int tsIndex, int tsStart)
{
  this->centroid = this->dataset.getTimeSeries(tsIndex, tsStart, tsStart + this->memberLength);
}

data_t Group::distanceFromCentroid(const TimeSeries& query, const dist_t distance, data_t dropout)
{
  data_t d = distance(this->centroid, query, dropout);
  return d;
}

candidate_time_series_t Group::getBestMatch(const TimeSeries& query, const dist_t warpedDistance) const
{
  member_coord_t currentMemberCoord = this->lastMemberCoord;

  data_t bestSoFarDist = INF;
  member_coord_t bestSoFarMember;

  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(currIndex, currStart, currStart + this->memberLength);
    data_t currentDistance = warpedDistance(query, currentTimeSeries, bestSoFarDist);

    if (currentDistance < bestSoFarDist)
    {
      bestSoFarDist = currentDistance;
      bestSoFarMember = currentMemberCoord;
    }

    currentMemberCoord = this->memberMap[currIndex * this->subTimeSeriesCount + currStart].prev;
  }

  int bestIndex = bestSoFarMember.first;
  int bestStart = bestSoFarMember.second;
  TimeSeries bestTimeSeries = this->dataset.getTimeSeries(bestIndex, bestStart, bestStart + this->memberLength);
  candidate_time_series_t best(bestTimeSeries, bestSoFarDist);

  return best;
}

vector<candidate_time_series_t> Group::intraGroupKSim(
    const TimeSeries& query, int k, const dist_t warpedDistance) const
{
  vector<candidate_time_series_t> bestSoFar;

  data_t bestSoFarDist = INF;
  member_coord_t bestSoFarMember;
  member_coord_t currentMemberCoord = this->lastMemberCoord;  

  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(currIndex, currStart, currStart + this->memberLength);
    if (k > 0) // directly add to best 
    {
      data_t currentDistance = warpedDistance(query, currentTimeSeries, INF);
      bestSoFar.push_back(candidate_time_series_t(currentTimeSeries, currentDistance));
      k -= 1;      
      if (k == 0) {
        // Heapify exactly once when the heap is filled.
        std::make_heap(bestSoFar.begin(), bestSoFar.end());
      }
    }
    else // heap is full, keep only best k'
    { 
      bestSoFarDist = bestSoFar.front().dist;
      data_t currentDistance = warpedDistance(query, currentTimeSeries, bestSoFarDist); 

      if (currentDistance < bestSoFarDist) 
      { 
        bestSoFar.push_back(candidate_time_series_t(currentTimeSeries, currentDistance));
        std::push_heap(bestSoFar.begin(), bestSoFar.end());
        std::pop_heap(bestSoFar.begin(), bestSoFar.end());
        bestSoFar.pop_back();
      } 
    }
    currentMemberCoord = this->memberMap[currIndex * this->subTimeSeriesCount + currStart].prev;    
  }
  return bestSoFar;
}

vector<TimeSeries> Group::getMembers() const
{
  vector<TimeSeries> members;
  member_coord_t currentMemberCoord = this->lastMemberCoord;
  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(currIndex, currStart, currStart + this->memberLength);
    members.push_back(currentTimeSeries);
    currentMemberCoord = this->memberMap[currIndex * this->subTimeSeriesCount + currStart].prev;
  }
  return members;
}

void Group::saveGroup(ofstream &fout) const
{
  // Group count
  // Group centroid
  // Members in the group, represented by <index, start> pairs
  this->centroid.printData(fout); fout << endl;
  fout << this->count << " ";
  member_coord_t currentMemberCoord = this->lastMemberCoord;
  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;
    fout << currIndex << " " << currStart << " ";
    currentMemberCoord = this->memberMap[currIndex * this->subTimeSeriesCount + currStart].prev;    
  }
  fout << endl;
}

void Group::loadGroup(ifstream &fin)
{
  int cnt;
  this->centroid = TimeSeries(this->memberLength);

  for (int i = 0; i < this->memberLength; i++) {
    fin >> this->centroid[i];
  }

  fin >> cnt; 
  int index, start;
  for (int i = 0; i < cnt; i++) {
    fin >> index >> start;
    this->addMember(index, start);
  }
}

} // namespace genex

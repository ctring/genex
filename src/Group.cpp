#include "Group.hpp"

#include <vector>
#include <functional>
#include <queue>

#include "TimeSeries.hpp"
#include "distance/Distance.hpp"

namespace genex {

void Group::addMember(int tsIndex, int tsStart)
{
  this->count++;
  this->memberMap[tsIndex * this->subTimeSeriesCount + tsStart] =
    group_membership_t(this->groupIndex, this->lastMemberCoord);
  this->lastMemberCoord = std::make_pair(tsIndex, tsStart);

  TimeSeries newMember = this->dataset.getTimeSeries(tsIndex, tsStart, tsStart + this->memberLength);
}

void Group::setCentroid(int tsIndex, int tsStart)
{
  this->centroid = this->dataset.getTimeSeries(tsIndex, tsStart, tsStart + this->memberLength);
}

data_t Group::distanceFromCentroid(const TimeSeries& query, const dist_t distance, data_t dropout)
{
  return distance(this->centroid, query, dropout);
}

candidate_time_series_t Group::getBestMatch(const TimeSeries& query, const dist_t warpedDistance) const
{
  // TODO: warpedDistance should be bestSoFarDist?
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

const std::priority_queue<candidate_time_series_t, std::vector<candidate_time_series_t>, 
    pless<candidate_time_series_t>>& Group::intraGroupKNN(const TimeSeries& query, int k, const dist_t warpedDistance) const
{
  std::priority_queue<candidate_time_series_t, std::vector<candidate_time_series_t>, 
    pless<candidate_time_series_t>> bestSoFar;

  data_t bestSoFarDist = INF;
  member_coord_t bestSoFarMember;
  
  member_coord_t currentMemberCoord = this->lastMemberCoord;  

  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(currIndex, currStart, currStart + this->memberLength);
    
    if (k > 0) // add to best 
    {
      data_t currentDistance = warpedDistance(query, currentTimeSeries, INF);
      bestSoFar.push(candidate_time_series_t(currentTimeSeries, currentDistance));
      k -= 1;
    }
    else //
    {
      bestSoFarDist = bestSoFar.top().dist;
      data_t currentDistance = 3.4; //warpedDistance(query, currentTimeSeries, bestSoFarDist);
      if (currentDistance < bestSoFarDist) 
      {
        bestSoFar.push(candidate_time_series_t(currentTimeSeries, currentDistance));
      }
      bestSoFar.pop(); // remove worst
    }
  }
  return bestSoFar;
}

const std::vector<TimeSeries> Group::getMembers() const
{
  std::vector<TimeSeries> members;
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


} // namespace genex

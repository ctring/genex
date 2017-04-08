#include "Group.hpp"
#include "distance/Distance.hpp"

namespace genex {

void Group::addMember(int tsIndex, int tsStart)
{
  this->count++;
  this->memberMap[tsIndex][tsStart] = group_membership_t(this->groupIndex, this->lastMemberCoord);
  this->lastMemberCoord = std::make_pair(tsIndex, tsStart);

  TimeSeries newMember = this->dataset.getTimeSeries(tsIndex, tsStart, tsStart + this->memberLength);
  this->centroid.addTimeSeries(newMember);
}

data_t Group::distanceFromCentroid(const TimeSeries& query, const DistanceMetric* metric, data_t dropout)
{
  return distance::generalDistance(metric, this->centroid, query, dropout);
}

data_t Group::warpDistanceFromCentroid(const TimeSeries& query, const DistanceMetric* metric, data_t dropout)
{
  return distance::generalWarpedDistance(metric, this->centroid, query, dropout);
}

candidate_time_series_t Group::getBestMatch(const TimeSeries& query, const DistanceMetric* metric) const
{
  member_coord_t currentMemberCoord = this->lastMemberCoord;

  data_t bestSoFarDist = INF;
  member_coord_t bestSoFarMember;

  while (currentMemberCoord.first != -1)
  {
    int currIndex = currentMemberCoord.first;
    int currStart = currentMemberCoord.second;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(currIndex, currStart, currStart + this->memberLength);
    data_t currentDistance = distance::generalWarpedDistance(metric, query, currentTimeSeries, bestSoFarDist);

    if (currentDistance < bestSoFarDist)
    {
      bestSoFarDist = currentDistance;
      bestSoFarMember = currentMemberCoord;
    }

    currentMemberCoord = this->memberMap[currIndex][currStart].prev;
  }

  int bestIndex = bestSoFarMember.first;
  int bestStart = bestSoFarMember.second;
  TimeSeries bestTimeSeries = this->dataset.getTimeSeries(bestIndex, bestStart, bestStart + this->memberLength);
  candidate_time_series_t best(bestTimeSeries, bestSoFarDist);

  return best;
}

} // namespace genex

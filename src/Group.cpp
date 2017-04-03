#include "Group.hpp"
#include "distance/Distance.hpp"

namespace genex {

Group::~Group()
{
  int groupCount = this->getCount();
  node_t* last = this->lastMember;
  node_t* next;

  for (int i = 0; i < this->count; i++ )
  {
    next = last->next;
    delete last;
    last = next;
  }
}

void Group::addMember(int index, int start)
{
  this->count++;
  this->lastMember = new node_t(index, start, this->lastMember);
  TimeSeries newMember = this->dataset.getTimeSeries(index, start, start + this->memberLength);
  this->centroid.addTimeSeries(newMember);
}

data_t Group::distanceFromCentroid(const TimeSeries& query, const DistanceMetric* metric, data_t dropout)
{
  return distance::generalWarpedDistance(metric, this->centroid, query, dropout);
}

candidate_time_series_t Group::getBestMatch(const TimeSeries& query, const DistanceMetric* metric) const
{
  node_t* currentNode = this->lastMember;
  data_t bestSoFarDist = INF;
  node_t* bestSoFarNode;

  while (currentNode != nullptr)
  {
    int index = currentNode->index;
    int start = currentNode->start;

    TimeSeries currentTimeSeries = this->dataset.getTimeSeries(index, start, start + this->memberLength);
    data_t currentDistance = distance::generalWarpedDistance(metric, query, currentTimeSeries, bestSoFarDist);

    if (currentDistance < bestSoFarDist)
    {
      bestSoFarDist = currentDistance;
      bestSoFarNode = currentNode;
    }

    currentNode = currentNode->next;
  }

  int bestIndex = bestSoFarNode->index;
  int bestStart = bestSoFarNode->start;
  TimeSeries bestTimeSeries = this->dataset.getTimeSeries(bestIndex, bestStart, bestStart + this->memberLength);
  candidate_time_series_t best(bestTimeSeries, bestSoFarDist);

  return best;
}

} // namespace genex

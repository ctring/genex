#include "Group.hpp"
#include "distance/Distance.hpp"
#include <iostream> //debug
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

candidate_t Group::getBestMatch(const TimeSeries& query, const DistanceMetric* metric, data_t dropout)
{
  int index, start;
  data_t curr_d = 0;
  candidate_t bsf(dropout);
  node_t* current_node = this->lastMember;

  for(int g = 0; g < this->count; g++)
  {
    index = current_node->index;
    start = current_node->start;
    // this may not work because its a temporary object
    TimeSeries curr = this->dataset.getTimeSeries(index, start, start + this->memberLength);
    curr_d = distance::generalWarpedDistance(metric, query, curr, bsf.dist);

    if (curr_d < bsf.dist)
    {
      bsf.data = &curr;
      bsf.dist = curr_d;
    }

    current_node = current_node->next;
  }

  return bsf;
}

} // namespace genex

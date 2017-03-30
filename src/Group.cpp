#include "Group.hpp"
#include "distance/Distance.hpp"
#include <iostream> //debug
namespace genex {

void Group::addMember(int index, int start)
{
  count++;
  this->lastMember = new node_t(index, start, this->lastMember);
  centroid.addTimeSeries(this->dataset.getTimeSeries(index, start, start + this->memberLength));
}

data_t Group::distance(int len, const TimeSeries& query, const DistanceMetric* metric)
{
  return distance::generalWarpedDistance(metric, this->centroid, query, len);
}

candidate_t Group::getBestMatch(const TimeSeries& query, const DistanceMetric* metric)
{
  int index, start;
  data_t curr_d = 0;
  candidate_t bsf(INF);
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

int Group::next_id = 0;

} // namespace genex

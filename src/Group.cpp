#include "Group.hpp"
#include "distance/Distance.hpp"
namespace genex {

void Group::addMember(int index, int start)
{
  count++;

  //set the membership & add values to cendtroid
  memberMap[index * repLength + start].data = this->id;
  centroid.addTimeSeries(this->dataset.getTimeSeries(index, start, start + this->memberLength));

  // track locations of members (reverse list)
  memberMap[index * repLength + start].next = lastMember;
  this->lastMember = &memberMap[index * repLength + start];
}

bool Group::isMember(int index, int start) const
{
  return  memberMap[index * this->repLength + start].data == this->id;
}

data_t Group::distance(int len, const TimeSeries& query, const DistanceMetric& metric)
{
  return generalWarpedDistance(metric, this->centroid, query, len);
}

candidate_t Group::getBestMatch(int len, const TimeSeries& query, const DistanceMetric& metric)
{
  data_t curr_d = 0;
  candidate_t bsf(-1);

  for (int index = 0; index < dataset.getItemCount(); index++)
  {
    for (int start = 0; start < this->repLength; start++)
    {
      if (isMember(index, start))
      {
        TimeSeries curr = this->dataset.getTimeSeries(index, start, start + memberLength - 1);
        curr_d = generalWarpedDistance(metric, query, curr, memberLength);
        if (curr_d < bsf.dist)
        {
          bsf.data = &curr;
          bsf.dist = curr_d;
        }
      }
    }
  }

  return bsf;
}

int Group::next_id = 0;

} // namespace genex

#include "GroupsEqualLengthSet.hpp"
#include "GroupsEqualLength.hpp"

#include <vector>
#include <iostream> //debug
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/DistanceMetric.hpp"
#include "Group.hpp"

namespace genex {

  int GroupsEqualLengthSet::group(DistanceMetric* metric)
  {
    reset();
    this->groupsEqualLength.resize(dataset.getItemLength(), NULL);

    int count = 0;

    this->groupsEqualLength[0] = new GroupsEqualLength(dataset, 1);

    for (unsigned int i = 1; i < this->groupsEqualLength.size(); i++)
    { // Don't group zero length.
        this->groupsEqualLength[i] = new GroupsEqualLength(dataset, i+1);
        this->groupsEqualLength[i]->genGroups(metric, threshold);
        count += this->groupsEqualLength[i]->getCount();
    }
    
    return count;
  }

  candidate_t GroupsEqualLengthSet::getBestMatch(const TimeSeries& data, DistanceMetric* metric)
  {
    data_t bsf = INF;
    int bsfGroup = -1;
    int bsfLen = -1;

    for (int i = 0; i < this->groupsEqualLength.size(); i++)
    {
      // this finds the best set of groups of a length
      data_t dist = -1;

      // this looks through each group of a certain length finding the best of those groups and setting dist
      int g = this->groupsEqualLength[i]->getBestGroup(data, metric, dist, bsf);

      //  by ref
      std::cout << " post gg " << dist << std::endl;

      if ((dist < bsf) || (bsf == INF))
      {
        bsf = dist;
        bsfGroup = g;
        bsfLen = i + 1;
      }
    }

    return this->groupsEqualLength[bsfLen - 1]->getGroup(bsfGroup)->getBestMatch(data, metric);
  }

  void GroupsEqualLengthSet::reset(void)
  {
    for (unsigned int i = 0; i < this->groupsEqualLength.size(); i++)
      delete this->groupsEqualLength[i];

    this->groupsEqualLength.clear();
  }

  bool GroupsEqualLengthSet::valid(void)
  {
    return groupsEqualLength.size() > 0;
  }

} //genex

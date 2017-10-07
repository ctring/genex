#include "GroupableTimeSeriesSet.hpp"
#include "GroupsEqualLengthSet.hpp"
#include "Exception.hpp"
#include "distance/Distance.hpp"

#include <fstream>

using std::ofstream;
using std::endl;

namespace genex {

GroupableTimeSeriesSet::~GroupableTimeSeriesSet()
{
  this->resetGrouping();
}

int GroupableTimeSeriesSet::groupAllLengths(const std::string& distance_name, data_t threshold)
{
  if (!this->isLoaded())
  {
    throw GenexException("No data to group");
  }

  // clear old groups
  resetGrouping();

  this->groupsAllLengthSet = new GroupsEqualLengthSet(*this);
  int cntGroups = this->groupsAllLengthSet->group(distance_name, threshold);

  return cntGroups;
}

bool GroupableTimeSeriesSet::isGrouped() const
{
  return this->groupsAllLengthSet != nullptr;
}

void GroupableTimeSeriesSet::resetGrouping()
{
  delete this->groupsAllLengthSet;
  this->groupsAllLengthSet = nullptr;
}

void GroupableTimeSeriesSet::saveGroups(const string& path, bool groupSizeOnly) const
{
  if (!this->isGrouped()) {
    throw GenexException("No group found");
  }

  ofstream fout(path);
  if (fout)
  {
    // Version of the file format and the required dataset dimensions
    fout << 1 << " " << this->getItemCount() << " " << this->getItemLength() << endl;
    this->groupsAllLengthSet->saveGroups(fout, groupSizeOnly);
    fout.close();
  }
  else
  {
    throw GenexException("Cannot open file");
  }
}


candidate_time_series_t GroupableTimeSeriesSet::getBestMatch(const TimeSeries& query) const
{
  if (this->groupsAllLengthSet) //not nullptr
  {
    return this->groupsAllLengthSet->getBestMatch(query);
  }
  throw GenexException("Dataset is not grouped");
}

std::vector<TimeSeries> GroupableTimeSeriesSet::kNN(const TimeSeries& query, int k)
{
  if (this->groupsAllLengthSet) //not nullptr
  {
    return this->groupsAllLengthSet->kNN(query, k);
  }
  throw GenexException("Dataset is not grouped");
}


} // namespace genex
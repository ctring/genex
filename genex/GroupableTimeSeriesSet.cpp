#include <iostream>
#include <fstream>

#include "IO.hpp"
#include "GroupableTimeSeriesSet.hpp"
#include "group/GlobalGroupSpace.hpp"
#include "Exception.hpp"
#include "distance/Distance.hpp"

using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;

namespace genex {

GroupableTimeSeriesSet::~GroupableTimeSeriesSet()
{
  this->reset();
}

int GroupableTimeSeriesSet::groupAllLengths(const std::string& distanceName, data_t threshold, int numThreads)
{
  if (!this->isLoaded())
  {
    throw GenexException("No data to group");
  }

  // clear old groups
  reset();

  this->groupsAllLengthSet = new GlobalGroupSpace(*this);
  int cntGroups;
  if (numThreads == 1) {
    cntGroups = this->groupsAllLengthSet->group(distanceName, threshold);
  }
  else if (numThreads > 1) {
    cntGroups = this->groupsAllLengthSet->groupMultiThreaded(distanceName, threshold, numThreads);    
  }
  else {
    throw GenexException("Number of threads must be positive");    
  }
  return cntGroups;
}

bool GroupableTimeSeriesSet::isGrouped() const
{
  return this->groupsAllLengthSet != nullptr;
}

int GroupableTimeSeriesSet::getTotalNumberOfGroups() const
{
  if (this->isGrouped())
  {
    return this->groupsAllLengthSet->getTotalNumberOfGroups();
  }
  return 0;
}


string GroupableTimeSeriesSet::getDistanceName() const
{
  if (this->isGrouped())
  {
    return this->groupsAllLengthSet->getDistanceName();
  }
  return "";
}

data_t GroupableTimeSeriesSet::getThreshold() const
{
  if (this->isGrouped())
  {
    return this->groupsAllLengthSet->getThreshold();
  }
  return 0;
}


void GroupableTimeSeriesSet::reset()
{
  delete this->groupsAllLengthSet;
  this->groupsAllLengthSet = nullptr;
}

void GroupableTimeSeriesSet::saveGroupsOld(const string& path, bool groupSizeOnly) const
{
  if (!this->isGrouped()) {
    throw GenexException("No group found");
  }

  ofstream fout(path);
  if (fout)
  {
    // Version of the file format, the threshold and the required dataset dimensions
    fout << GROUP_FILE_VERSION << " "
         << this->getThreshold() << " "
         << this->getItemCount() << " "
         << this->getItemLength() << endl;
    this->groupsAllLengthSet->saveGroupsOld(fout, groupSizeOnly);
  }
  else
  {
    throw GenexException("Cannot open file");
  }
}

int GroupableTimeSeriesSet::loadGroupsOld(const string& path)
{
  int numberOfGroups = 0;
  ifstream fin(path);
  if (fin)
  {
    int version, grpItemCount, grpItemLength;
    data_t threshold;
    fin >> version >> threshold >> grpItemCount >> grpItemLength;
    if (version != GROUP_FILE_VERSION)
    {
      throw GenexException("Incompatible file version");
    }
    if (grpItemCount != this->getItemCount())
    {
      throw GenexException("Incompatible item count");
    }
    if (grpItemLength != this->getItemLength())
    {
      throw GenexException("Incompatible item length");
    }
    cout << "Saved groups are compatible with the dataset" << endl;
    reset();
    this->groupsAllLengthSet = new GlobalGroupSpace(*this);
    numberOfGroups = this->groupsAllLengthSet->loadGroupsOld(fin);
  }
  else
  {
    throw GenexException("Cannot open file");
  }
  return numberOfGroups;
}

candidate_time_series_t GroupableTimeSeriesSet::getBestMatch(const TimeSeries& query) const
{
  if (this->groupsAllLengthSet) //not nullptr
  {
    return this->groupsAllLengthSet->getBestMatch(query);
  }
  throw GenexException("Dataset is not grouped");
}

std::vector<candidate_time_series_t> GroupableTimeSeriesSet::kSim(const TimeSeries& query, int k, int h)
{
  if (this->groupsAllLengthSet) //not nullptr
  {
    if (h < k) {
      throw GenexException("Number of examined time series must be larger than "
                           "or equal to the number of time series to look for");
    }
    std::vector<candidate_time_series_t> results = this->groupsAllLengthSet->kSim(query, h);
    std::sort(results.begin(), results.end());
    results.resize(k);
    return results;
  }
  throw GenexException("Dataset is not grouped");
}


} // namespace genex
#include "GenexAPI.hpp"

#include "Exception.hpp"
#include "GroupableTimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "IO.hpp"

#include <vector>
#include <iostream>

using std::string;
using std::vector;

namespace genex {

GenexAPI::~GenexAPI()
{
  unloadAllDataset();
}

dataset_metadata_t GenexAPI::loadDataset(const string& name
                                        , const string& filePath
                                        , const string& separators
                                        , int maxNumRow
                                        , int startCol
                                        , bool hasNameCol)
{
  // Check if name is already used
  if (this->_loadedDatasets.find(name) != this->_loadedDatasets.end())
  {
    throw GenexException("Name has already been used");
  }

  auto newSet = new GroupableTimeSeriesSet();
  try {
    newSet->loadData(filePath, maxNumRow, startCol, separators, hasNameCol);
  } 
  catch (GenexException& e)
  {
    delete newSet;
    throw e;
  }

  this->_loadedDatasets[name] = newSet;
  this->_datasetCount++;

  return this->getDatasetInfo(name);
}

void GenexAPI::saveDataset(const string& name, const string& filePath, char separator)
{
  this->_checkDatasetName(name);
  this->_loadedDatasets[name]->saveData(filePath, separator);
}

void GenexAPI::unloadDataset(const string& name)
{
  this->_checkDatasetName(name);

  delete this->_loadedDatasets[name];
  this->_loadedDatasets[name] = nullptr;
  this->_loadedDatasets.erase(name);
  this->_datasetCount--;
}

void GenexAPI::unloadAllDataset()
{
  for (auto const& pair : this->_loadedDatasets)
  {
    delete this->_loadedDatasets[pair.first];
  }
  this->_loadedDatasets.clear();
  this->_datasetCount = 0;
}

int GenexAPI::getDatasetCount()
{
  return this->_datasetCount;
}

string GenexAPI::getTimeSeriesName(const string& name, int index)
{
  this->_checkDatasetName(name);
  return this->_loadedDatasets[name]->getTimeSeriesName(index);
}

dataset_metadata_t GenexAPI::getDatasetInfo(const string& name)
{
  this->_checkDatasetName(name);

  auto dataset = this->_loadedDatasets[name];
  return dataset_metadata_t(name,
                            dataset->getItemCount(),
                            dataset->getItemLength(),
                            dataset->isNormalized(),                            
                            dataset->isGrouped(),
                            dataset->getDistanceName(),
                            dataset->getThreshold());
}

vector<dataset_metadata_t> GenexAPI::getAllDatasetInfo()
{
  vector<dataset_metadata_t> info;
  for (auto const& pair : this->_loadedDatasets)
  {
    info.push_back(getDatasetInfo(pair.first));
  }
  return info;
}

vector<distance_metadata_t> GenexAPI::getAllDistanceInfo()
{
  const vector<string>& allDistanceName = getAllDistanceName();
  vector<distance_metadata_t> info;
  for (auto name : allDistanceName)
  {
    info.push_back(distance_metadata_t(name, ""));
  }
  return info;
}

TimeSeries GenexAPI::getTimeSeries(const string& name, int idx, int start, int end)
{
  this->_checkDatasetName(name);
  return this->_loadedDatasets[name]->getTimeSeries(idx, start, end);
}


std::pair<data_t, data_t> GenexAPI::normalizeDataset(const string& name)
{
  this->_checkDatasetName(name);
  return this->_loadedDatasets[name]->normalize();
}

int GenexAPI::groupDataset(const string& name, data_t threshold, const string& distance_name, int numThreads)
{
  this->_checkDatasetName(name);
  return this->_loadedDatasets[name]->groupAllLengths(distance_name, threshold, numThreads);
}

void GenexAPI::saveGroups(const string& name, const string& path)
{
  this->_checkDatasetName(name);
  saveToFile(*(this->_loadedDatasets[name]), path);
}

int GenexAPI::loadGroups(const string& name, const string& path)
{
  this->_checkDatasetName(name);
  loadFromFile(*(this->_loadedDatasets[name]), path);
  return this->_loadedDatasets[name]->getTotalNumberOfGroups();
}

void GenexAPI::saveGroupsOld(const string& name, const string &path, bool groupSizeOnly)
{
  this->_checkDatasetName(name);
  this->_loadedDatasets[name]->saveGroupsOld(path, groupSizeOnly);
}

int GenexAPI::loadGroupsOld(const string& name, const string& path)
{
  this->_checkDatasetName(name);
  return this->_loadedDatasets[name]->loadGroupsOld(path);
}

void GenexAPI::setWarpingBandRatio(double ratio)
{
  genex::setWarpingBandRatio(ratio);
}

candidate_time_series_t GenexAPI::getBestMatch(const string& target_name, const string& query_name,
                                               int index, int start, int end)
{
  this->_checkDatasetName(target_name);
  this->_checkDatasetName(query_name);

  const TimeSeries& query = _loadedDatasets[query_name]->getTimeSeries(index, start, end);
  return _loadedDatasets[target_name]->getBestMatch(query);
}

vector<candidate_time_series_t> 
GenexAPI::getKBestMatches(int k, int ke, const string& target_name, const string& query_name,
                          int index, int start, int end)
{
  this->_checkDatasetName(target_name);
  this->_checkDatasetName(query_name);

  const TimeSeries& query = _loadedDatasets[query_name]->getTimeSeries(index, start, end);
  return _loadedDatasets[target_name]->kSim(query, k, ke);
}

vector<candidate_time_series_t>
GenexAPI::getKBestMatchesBruteForce(int k, const string& target_name, const string& query_name,
                                    int index, int start, int end, int PAABlockSize)
{
  this->_checkDatasetName(target_name);
  this->_checkDatasetName(query_name);

  const TimeSeries& query = _loadedDatasets[query_name]->getTimeSeries(index, start, end);
  return _loadedDatasets[target_name]->kSimRaw(query, k, PAABlockSize);
}

// TODO: fix this function and also that in the brute force k best match
// dataset_metadata_t GenexAPI::PAA(int idx, int n)
// {
//   this->_checkDatasetName(idx);
//   this->_loadedDatasets[idx]->PAA(n);
//   return this->getDatasetInfo(idx);
// }

data_t GenexAPI::distanceBetween(const string& name1, int idx1, int start1, int end1,
                                 const string& name2, int idx2, int start2, int end2,
                                 const string& distance_name)
{
  this->_checkDatasetName(name1);
  this->_checkDatasetName(name2);
  TimeSeries ts1 = this->_loadedDatasets[name1]->getTimeSeries(idx1, start1, end1);
  TimeSeries ts2 = this->_loadedDatasets[name2]->getTimeSeries(idx2, start2, end2);  
  const dist_t distance = getDistanceFromName(distance_name);
  return distance(ts1, ts2, INF);
}

void GenexAPI::_checkDatasetName(const string& name) const
{ 
  if (this->_loadedDatasets.find(name) == this->_loadedDatasets.end())
  {
    throw GenexException("There is no dataset with the given name");
  }
}

} // namespace genex

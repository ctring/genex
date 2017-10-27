#include "GenexAPI.hpp"

#include "Exception.hpp"
#include "GroupableTimeSeriesSet.hpp"
#include "distance/Distance.hpp"

using std::string;
using std::vector;

namespace genex {

GenexAPI::~GenexAPI()
{
  unloadAllDataset();
}

dataset_info_t GenexAPI::loadDataset(const string& filePath, int maxNumRow,
                                     int startCol, const string& separators)
{

  GroupableTimeSeriesSet* newSet = new GroupableTimeSeriesSet();
  try {
    newSet->loadData(filePath, maxNumRow, startCol, separators);
  } catch (GenexException& e)
  {
    delete newSet;
    throw e;
  }

  int nextIndex = -1;
  for (unsigned int i = 0; i < this->loadedDatasets.size(); i++)
  {
    if (this->loadedDatasets[i] == nullptr)
    {
      nextIndex = i;
      break;
    }
  }

  if (nextIndex < 0) {
    nextIndex = this->loadedDatasets.size();
    this->loadedDatasets.push_back(nullptr);
  }

  this->loadedDatasets[nextIndex] = newSet;
  this->datasetCount++;

  return this->getDatasetInfo(nextIndex);
}

void GenexAPI::saveDataset(int index, const string& filePath, char separator)
{
  this->_checkDatasetIndex(index);
  this->loadedDatasets[index]->saveData(filePath, separator);
}

void GenexAPI::unloadDataset(int index)
{
  this->_checkDatasetIndex(index);

  delete loadedDatasets[index];
  loadedDatasets[index] = nullptr;
  if (index == loadedDatasets.size() - 1)
  {
    loadedDatasets.pop_back();
  }
  this->datasetCount--;
}

void GenexAPI::unloadAllDataset()
{
  for (unsigned int i = 0; i < this->loadedDatasets.size(); i++)
  {
    delete this->loadedDatasets[i];
  }
  this->loadedDatasets.clear();
  this->datasetCount = 0;
}

int GenexAPI::getDatasetCount()
{
  return this->datasetCount;
}

dataset_info_t GenexAPI::getDatasetInfo(int index)
{
  this->_checkDatasetIndex(index);

  GroupableTimeSeriesSet* dataset = this->loadedDatasets[index];
  return dataset_info_t(index,
                        dataset->getFilePath(),
                        dataset->getItemCount(),
                        dataset->getItemLength(),
                        dataset->isGrouped(),
                        dataset->isNormalized());
}

vector<dataset_info_t> GenexAPI::getAllDatasetInfo()
{
  vector<dataset_info_t> info;
  for (unsigned int i = 0; i < this->loadedDatasets.size(); i++)
  {
    if (loadedDatasets[i] != nullptr)
    {
      info.push_back(getDatasetInfo(i));
    }
  }
  return info;
}

vector<distance_info_t> GenexAPI::getAllDistanceInfo()
{
  const vector<string>& allDistanceName = getAllDistanceName();
  vector<distance_info_t> info;
  for (auto name : allDistanceName)
  {
    info.push_back(distance_info_t(name, ""));
  }
  return info;
}

std::pair<data_t, data_t> GenexAPI::normalizeDataset(int idx)
{
  this->_checkDatasetIndex(idx);
  return this->loadedDatasets[idx]->normalize();
}

int GenexAPI::groupDataset(int index, data_t threshold, const string& distance_name, int numThreads)
{
  this->_checkDatasetIndex(index);
  return this->loadedDatasets[index]->groupAllLengths(distance_name, threshold, numThreads);
}

void GenexAPI::saveGroup(int index, const string &path, bool groupSizeOnly)
{
  this->_checkDatasetIndex(index);
  this->loadedDatasets[index]->saveGroups(path, groupSizeOnly);
}

int GenexAPI::loadGroup(int index, const string& path)
{
  this->_checkDatasetIndex(index);
  return this->loadedDatasets[index]->loadGroups(path);
}

void GenexAPI::setWarpingBandRatio(double ratio)
{
  genex::setWarpingBandRatio(ratio);
}

candidate_time_series_t GenexAPI::getBestMatch(int result_idx, int query_idx, int index, int start, int end)
{
  this->_checkDatasetIndex(result_idx);
  this->_checkDatasetIndex(query_idx);

  const TimeSeries& query = loadedDatasets[query_idx]->getTimeSeries(index, start, end);
  return loadedDatasets[result_idx]->getBestMatch(query);
}

vector<candidate_time_series_t> GenexAPI::kSim(int k, int h, int result_idx, int query_idx, int index, int start, int end)
{
  this->_checkDatasetIndex(result_idx);
  this->_checkDatasetIndex(query_idx);

  const TimeSeries& query = loadedDatasets[query_idx]->getTimeSeries(index, start, end);
  return loadedDatasets[result_idx]->kSim(query, k, h);
}

vector<candidate_time_series_t> GenexAPI::kSimRaw(int k, int result_idx, int query_idx, int index, int start, int end, int PAABlockSize)
{
  this->_checkDatasetIndex(result_idx);
  this->_checkDatasetIndex(query_idx);

  const TimeSeries& query = loadedDatasets[query_idx]->getTimeSeries(index, start, end);
  return loadedDatasets[result_idx]->kSimRaw(query, k, PAABlockSize);
}

dataset_info_t GenexAPI::PAA(int idx, int n)
{
  this->_checkDatasetIndex(idx);
  this->loadedDatasets[idx]->PAA(n);
  return this->getDatasetInfo(idx);
}

data_t GenexAPI::distanceBetween(int ds1, int idx1, int start1, int end1,
                                 int ds2, int idx2, int start2, int end2,
                                 const std::string& distance_name)
{
  this->_checkDatasetIndex(ds1);
  this->_checkDatasetIndex(ds2);
  TimeSeries ts1 = this->loadedDatasets[ds1]->getTimeSeries(idx1, start1, end1);
  TimeSeries ts2 = this->loadedDatasets[ds1]->getTimeSeries(idx2, start2, end2);  
  const dist_t distance = getDistance(distance_name);
  return distance(ts1, ts2, INF);
}

void GenexAPI::_checkDatasetIndex(int index)
{
  if (index < 0 || index >= loadedDatasets.size() || loadedDatasets[index] == nullptr)
  {
    throw GenexException("There is no dataset with given index");
  }
}

} // namespace genex

#include "GenexAPI.hpp"

#include "Exception.hpp"
#include "GroupableTimeSeriesSet.hpp"
#include "distance/Distance.hpp"

namespace genex {

GenexAPI::~GenexAPI()
{
  unloadAllDataset();
}

dataset_info_t GenexAPI::loadDataset(const std::string& filePath, int maxNumRow,
                                     int startCol, const std::string& separators)
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
                        dataset->getItemLength());
}

std::vector<dataset_info_t> GenexAPI::getAllDatasetInfo()
{
  std::vector<dataset_info_t> info;
  for (unsigned int i = 0; i < this->loadedDatasets.size(); i++)
  {
    if (loadedDatasets[i] != nullptr)
    {
      info.push_back(getDatasetInfo(i));
    }
  }
  return info;
}

std::vector<distance_info_t> GenexAPI::getAllDistanceInfo()
{
  const std::vector<std::string>& allDistanceName = getAllDistanceName();
  std::vector<distance_info_t> info;
  for (auto name : allDistanceName)
  {
    info.push_back(distance_info_t(name, ""));
  }
  return info;
}

int GenexAPI::groupDataset(int index, data_t threshold, const std::string& distance_name)
{
  this->_checkDatasetIndex(index);
  return this->loadedDatasets[index]->groupAllLengths(distance_name, threshold);
}

candidate_time_series_t GenexAPI::getBestMatch(int result_idx, int query_idx, int index)
{
  this->_checkDatasetIndex(result_idx);
  this->_checkDatasetIndex(query_idx);

  const TimeSeries& query = loadedDatasets[query_idx]->getTimeSeries(index);
  return loadedDatasets[result_idx]->getBestMatch(query);
}

candidate_time_series_t GenexAPI::getBestMatch(int result_idx, int query_idx, int index, int start, int end)
{
  this->_checkDatasetIndex(result_idx);
  this->_checkDatasetIndex(query_idx);

  const TimeSeries& query = loadedDatasets[query_idx]->getTimeSeries(index, start, end);
  return loadedDatasets[result_idx]->getBestMatch(query);
}

void GenexAPI::_checkDatasetIndex(int index)
{
  if (index < 0 || index >= loadedDatasets.size() || loadedDatasets[index] == nullptr)
  {
    throw GenexException("There is no dataset with given index");
  }
}

} // namespace genex

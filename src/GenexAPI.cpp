#include "GenexAPI.hpp"

#include "Exception.hpp"
#include "GroupableTimeSeriesSet.hpp"

namespace genex {

GenexAPI::~GenexAPI()
{
  unloadAllDataset();
}

int GenexAPI::loadDataset(const std::string& filePath, int maxNumRow,
                          const std::string& separators, int startCol)
{

  GroupableTimeSeriesSet* newSet = new GroupableTimeSeriesSet();
  try {
    newSet->loadData(filePath, maxNumRow, separators, startCol);
  } catch (GenexException& e)
  {
    delete newSet;
    throw e;
  }

  int nextIndex = -1;
  for (unsigned int i = 0; i < this->loadedDataset.size(); i++)
  {
    if (this->loadedDataset[i] == nullptr)
    {
      nextIndex = i;
      break;
    }
  }
  if (nextIndex < 0) {
    nextIndex = this->loadedDataset.size();
    this->loadedDataset.push_back(nullptr);
  }

  this->loadedDataset[nextIndex] = newSet;
  this->datasetCount++;

  return nextIndex;
}

void GenexAPI::unloadDataset(int index)
{
  if (index < 0 || index >= loadedDataset.size() || loadedDataset[index] == nullptr)
  {
    throw GenexException("No dataset with given index");
  }
  delete loadedDataset[index];
  loadedDataset[index] = nullptr;
  if (index == loadedDataset.size() - 1)
  {
    loadedDataset.pop_back();
  }
  this->datasetCount--;
}

void GenexAPI::unloadAllDataset()
{
  for (unsigned int i = 0; i < this->loadedDataset.size(); i++)
  {
    delete this->loadedDataset[i];
  }
  this->loadedDataset.clear();
  this->datasetCount = 0;
}

int GenexAPI::getDatasetCount()
{
  return this->datasetCount;
}

} // namespace genex

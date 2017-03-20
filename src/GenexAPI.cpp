#include <vector>
#include <string>

#include "Exception.hpp"
#include "GroupableTimeSeriesSet.hpp"

namespace genex {

static std::vector<GroupableTimeSeriesSet*> loadedDataset;
static int datasetCount = 0;

int loadDataset(const std::string& filePath, int maxNumRow,
                const std::string& separators = " ", int startCol = 0)
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
  for (unsigned int i = 0; i < loadedDataset.size(); i++)
  {
    if (loadedDataset[i] == NULL)
    {
      nextIndex = i;
      break;
    }
  }
  if (nextIndex < 0) {
    nextIndex = loadedDataset.size();
    loadedDataset.push_back(NULL);
  }

  loadedDataset[nextIndex] = newSet;

  datasetCount++;

  return nextIndex;
}

int getDatasetCount()
{
  return datasetCount;
}

} // namespace genex
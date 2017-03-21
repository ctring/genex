#ifndef GENEX_API_H
#define GENEX_API_H

#include <vector>
#include <string>

#include "GroupableTimeSeriesSet.hpp"

namespace genex {

class GenexAPI
{
public:

  /**
   *  @brief destructor
   */
  ~GenexAPI();

  /**
   *  @brief loads data from a text file to the memory
   *
   *  Values in the text file have to be arranged in a table format. The number of
   *  values (a.k.a columns) is inferred from the first line. All lines must have
   *  the same number of columns. If the number of lines exceeds maxNumRow, only
   *  maxNumRow lines are read and the rest is discarded. On the other hand, if
   *  maxNumRow is larger than or equal to the actual number of lines, or maxNumRow is
   *  not positive all lines are read.
   *
   *  @param filePath path to a text file
   *  @param maxNumRow maximum number of rows to be read. If this value is not positive,
   *         all lines are read. Default: 0
   *  @param separator a string containings possible separator characters for values
   *         in a line. Default: " "
   *  @param startCol columns before startCol are discarded. Default: 0
   *  @return an index used to refer to the just loaded dataset
   *
   *  @throw GenexException if cannot read from the given file
   */
  int loadDataset(const std::string& filePath, int maxNumRow = 0,
                  const std::string& separators = " ", int startCol = 0);

  /**
   *  @brief unloads a dataset at given index
   *
   *  @param index index of the dataset being unloaded
   */
  void unloadDataset(int index);

  /**
   *  @brief unloads all datasets
   */
  void unloadAllDataset();

  /**
   *  @brief gets the number of loaded datasets
   *
   *  @return number of loaded datasets
   */
  int getDatasetCount();

private:
  std::vector<GroupableTimeSeriesSet*> loadedDataset;
  int datasetCount = 0;
};

} // namespace genex

#endif
#ifndef GENEX_API_H
#define GENEX_API_H

#include <vector>
#include <string>

#include "GroupableTimeSeriesSet.hpp"

namespace genex {

/**
 * A struct holding general information of a dataset
 */
struct dataset_info_t
{
  dataset_info_t() : name(""), id(-1), itemCount(0), itemLength(0) {}
  dataset_info_t(int id, std::string name, int itemCount, int itemLength) :
    name(name), id(id), itemCount(itemCount), itemLength(itemLength) {}

  std::string name;
  int id;
  int itemCount;
  int itemLength;
};

/**
 * A struct holding general information of a distance metric
 */
struct distance_info_t
{
  distance_info_t(std::string name, std::string description) :
    name(name), description(description) {}
  std::string name;
  std::string description;
};

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
   *         all lines are read
   *  @param separator a string containings possible separator characters for values
   *         in a line
   *  @param startCol columns before startCol are discarded
   *  @return an index used to refer to the just loaded dataset
   *
   *  @throw GenexException if cannot read from the given file
   */
  dataset_info_t loadDataset(const std::string& filePath, int maxNumRow,
                             int startCol, const std::string& separators);

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
   *  @return number of loaded datasets
   */
  int getDatasetCount();

  /**
   *  @brief gets information of a dataset
   *
   *  @param index index of the dataset
   *  @return a dataset_info_t struct containing information about the dataset
   */
  dataset_info_t getDatasetInfo(int index);

  /**
   *  @return a vector of information of loaded dataset
   */
  std::vector<dataset_info_t> getAllDatasetInfo();

  /**
   *  @return a vector of names of available distance metrics
   */
  std::vector<distance_info_t> getAllDistanceInfo();

  /**
   *  @brief groups the dataset
   *
   *  @param the index of the dataset to use
   *  @param metric the distance metric to use when grouping the data
   *  @param threshold the threshold to use when creating the group
   *  @return the number of groups created
   */
  int groupDataset(int idx, data_t threshold, const std::string& distance_name);

  /**
   *  @brief gets the best match in a dataset
   *
   *  @param result_idx the index of the result dataset
   *  @param query_idx the index of the query dataset
   *  @param index the index of the timeseries in the query dataset
   *  @param start the start of the index
   *  @param end the end of the index
   *  @return best match in the dataset
   */
  candidate_time_series_t getBestMatch(int result_idx, int query_idx, int index, int start, int end);
  candidate_time_series_t getBestMatch(int result_idx, int query_idx, int index);

private:
  void _checkDatasetIndex(int index);

  std::vector<GroupableTimeSeriesSet*> loadedDatasets;
  int datasetCount = 0;
};

} // namespace genex

#endif // GENEX_API_H
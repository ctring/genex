#ifndef GENEX_API_H
#define GENEX_API_H

#include <map>
#include <string>
#include <vector>

#include "GroupableTimeSeriesSet.hpp"
#include "PAAWrapper.hpp"
#include "TimeSeries.hpp"

using std::string;
using std::vector;

namespace genex {

/**
 * A struct holding general information of a dataset
 */
struct dataset_metadata_t
{
  dataset_metadata_t() = default;
  dataset_metadata_t(
    string name
    , int itemCount
    , int itemLength
    , bool isNormalized
    , bool isGrouped
    , string distance
    , data_t threshold) :
      name(name),
      isNormalized(isNormalized),
      itemCount(itemCount),
      itemLength(itemLength),
      isGrouped(isGrouped),
      distance(distance),
      threshold(threshold) {}

  string name = "";
  int itemCount = 0;
  int itemLength = 0;
  bool isNormalized = false;
  bool isGrouped = false;
  string distance = "";
  data_t threshold = 0;
};

/**
 * A struct holding general information of a distance metric
 */
struct distance_metadata_t
{
  distance_metadata_t(
    string name,
    string description) : 
      name(name),
      description(description) {}

  string name;
  string description;
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
   *  maxNumRow lines are read and the rest is discarded. If maxNumRow is larger than 
   *  or equal to the actual number of lines, or maxNumRow is not positive, all lines 
   *  are read.
   *
   *  @param name name of the dataset
   *  @param filePath path to a text file
   *  @param separator a string containing possible separator characters for values
   *         in a line
   *  @param maxNumRow maximum number of rows to be read. If this value is not positive,
   *         all lines are read
   *  @param startCol columns before startCol, in 0-based index, are discarded
   *  @param hasNameCol whether the first column (starting from startCol) is the one
   *         with names for each time series
   *
   *  @return a dataset_metadata_t struct containing metadata of the dataset
   *
   *  @throw GenexException if cannot read from the given file
   */
  dataset_metadata_t loadDataset(const string& name
                                 , const string& filePath
                                 , const string& separators = " "
                                 , int maxNumRow = 0
                                 , int startCol = 0
                                 , bool hasNameCol = false);

  /*
   *  @brief saves data from memory to a file
   *
   *  @param name name of the dataset
   *  @param filePath path to a text file
   *  @param separator a character to separate entries in the file
   *
   *  @throw GenexException if cannot read from the given file
   */
  void saveDataset(const string& name, const string& filePath, char separator);                           

  /**
   *  @brief unloads a dataset at given name
   *
   *  @param name name of the dataset being unloaded
   */
  void unloadDataset(const string& name);

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
   *  @param name name of the dataset
   *  @return a dataset_metadata_t struct containing information about the dataset
   */
  dataset_metadata_t getDatasetInfo(const string& name);

  /**
   *  @brief gets name of a time series in a dataset
   *
   *  @param name name of the dataset
   *  @param index index of the time series
   *  @return name of the specified time series.
   */
  string getTimeSeriesName(const string& name, int index);

  /**
   *  @return a vector of information of loaded dataset
   */
  vector<dataset_metadata_t> getAllDatasetInfo();

  /**
   *  @return a vector of names of available distance metrics
   */
  vector<distance_metadata_t> getAllDistanceInfo();

  /**
   *  @param name dataset name of the time series
   *  @param idx index of the time series
   *  @param start starting position of the time series
   *  @param end ending position of the time series
   * 
   *  @return a time series
   */
  TimeSeries getTimeSeries(const string& name, int idx, int start, int end);

  /**
   *  @brief normalizes the dataset
   *
   *  Each value in the dataset is transformed by the following formula:
   *    d = (d - min) / (max - min)
   *  Where min and max are respectively the minimum and maximum values
   *  across the whole dataset.
   *
   *  @param name name of the dataset to be normalized
   *  @return a pair (min, max) - the minimum and maximum value across
   *          the whole dataset before being normalized.
   */
  std::pair<data_t, data_t> normalizeDataset(const string& name);

  /**
   *  @brief groups a dataset
   *
   *  @param name name of the dataset to be grouped
   *  @param threshold the threshold to use when creating the group
   *  @param distanceName the distance to use when grouping the data
   *  @param wholeSeriesOnly if set to true group the largest length only
   *  @return the number of groups created
   */
  int groupDataset(const string& name
                   , data_t threshold
                   , const string& distanceName = "euclidean"
                   , int numThreads = 1
                   , bool wholeSeriesOnly = false);

   /**
   *  @brief save all groups of a dataset to a file
   *  
   *  This function saves all groups of a dataset to a file. The dataset must be
   *  grouped before this function is used. The dimension of the dataset (e.g. size, length)
   *  is also included in the file so that compatibility can be checked later.
   * 
   *  @param name name of the dataset
   *  @param path path to the saved group file
   */
  void saveGroups(const string& name, const string& path);

  /**
   *  @brief load all groups of a dataset from a file.
   *
   *  This function load all saved groups from a file to a dataset. The dimension of
   *  the dataset (e.g. size, lenth) must match the dimensions saved in the group file. 
   *
   *  @param name name of the dataset
   *  @param path path to the group file
   *  @return number of groups
   */
  int loadGroups(const string& name, const string& path);

  /**
   *  @brief saves all groups of a dataset to a text file (deprecated).
   *  
   *  This function saves all groups of a dataset to a file. The dataset must be
   *  grouped before this function is used. The dimension of the dataset (e.g. size, length)
   *  is also included in the file so that compatibility can be checked later.
   * 
   *  @param name name of the dataset
   *  @param path path to the saved group file
   *  @param groupSizeOnly if set to true, only the sizes of the groups are saved.
   */
  void saveGroupsOld(const string& name, const string& path, bool groupSizeOnly);

  /**
   *  @brief loads all groups of a dataset from a text file (deprecated).
   *
   *  This function load all saved groups from a file to a dataset. The dimension of
   *  the dataset (e.g. size, lenth) must match the dimensions saved in the group file. 
   *
   *  @param name name of the dataset
   *  @param path path to the group file
   */
  int loadGroupsOld(const string& name, const string& path);

  /**
   *  @brief sets the ratio for the warping band for computing the Keogh's lowerbound. Default: 0.1 
   */
  void setWarpingBandRatio(double ratio);

  /**
   *  @brief gets a single similar time series to the query
   *
   *  @param target_name name of the target dataset
   *  @param query_name name of the query dataset
   *  @param index index of the query time series
   *  @param start start location in the query time series
   *  @param end end location in the query time series (exclusive)
   *  @return best match in the dataset
   */
  candidate_time_series_t 
  getBestMatch(const string& target_name
               , const string& query_name
               , int index
               , int start = -1
               , int end = -1);

  /**
   *  @brief gets k similar time series to the query
   * 
   *  @param k number of similar time series to find
   *  @param ke number of time series to examine
   *  @param target_name name of the result dataset
   *  @param query_name name of the query dataset
   *  @param index index of the query time series
   *  @param start start location in the query time series
   *  @param end end location in the query time series (exclusive)
   *  @return k best match in the dataset
   */
  vector<candidate_time_series_t> 
  getKBestMatches(int k
                  , int ke
                  , const string& target_name
                  , const string& query_name
                  , int index
                  , int start = -1
                  , int end = -1);

 /**
   *  @brief gets k similar TimeSeries to the query, exhaustively.
   *  Provides the exact distance.
   * 
   *  @param k the number of similar time series to find
   *  @param target_name the index of the result dataset
   *  @param query_name the index of the query dataset
   *  @param index the index of the timeseries in the query dataset
   *  @param start the start of the index
   *  @param end the end of the index
   *  @param distance distance used in computing DTW
   *  @return k similar time series
   */
  vector<candidate_time_series_t>
  getKBestMatchesBruteForce(int k
                            , const string& target_name
                            , const string& query_name
                            , int index
                            , int start = -1
                            , int end = -1
                            , const string& distance = "euclidean");


  /**
   * @brief prepare a dataset for matching with PAA
   * @param name name of a dataset
   * @blockSize block size used for PAA
   */
  void preparePAA(const string& name, int blockSize);

 /**
   *  @brief gets k similar TimeSeries to the query, using PAA.
   *  Provides approximated distance.
   * 
   *  @param k the number of similar time series to find
   *  @param target_name the index of the result dataset
   *  @param query_name the index of the query dataset
   *  @param index the index of the timeseries in the query dataset
   *  @param start the start of the index
   *  @param end the end of the index
   *  @param distance distance used in computing DTW.
   *  @return k similar time series
   */
  vector<candidate_time_series_t>
  getKBestMatchesPAA(int k
                     , const string& target_name
                     , const string& query_name
                     , int index
                     , int start = -1
                     , int end = -1
                     , const string& distance = "euclidean");

  /**
   *  @brief computes the distance between 2 time series.
   * 
   *  @param name1 dataset name of the first time series
   *  @param idx1 index of the first time series
   *  @param start1 starting position of the first time series
   *  @param end1 ending position of the first time series
   *  @param name2 dataset name of the second time series
   *  @param idx2 index of the second time series
   *  @param start2 starting position of the second time series
   *  @param end2 ending position of the second time series
   *  @param distanceName name of the distance being used in the calculation
   *  @return distance between the 2 time series
   */
  data_t distanceBetween(const string& name1, int idx1, int start1, int end1
                         , const string& name2, int idx2, int start2, int end2
                         , const string& distanceName);

private:
  void _checkDatasetName(const string& name) const;

  std::map<string, GroupableTimeSeriesSet*> _loadedDatasets;
  std::map<string, PAAWrapper*> _paaWrappers;
  int _datasetCount = 0;
};

} // namespace genex

#endif // GENEX_API_H
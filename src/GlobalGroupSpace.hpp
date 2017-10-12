#ifndef GLOBAL_GROUP_SPACE_H
#define GLOBAL_GROUP_SPACE_H

#include "LocalLengthGroupSpace.hpp"
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

#include <vector>
#include <fstream>

namespace genex {

/**
 *  The set of all groups of equal lengths for a dataset
 */
class GlobalGroupSpace
{
public:

  /**
   *  @brief The constructor for the GlobalGroupSpace
   *
   *  @param dataset the dataset to group
   *  @param threshold the threshold for similarity within a group
   */
  GlobalGroupSpace(const TimeSeriesSet& dataset) : dataset(dataset) {};

  /**
   *  @brief The deconstructor for the GlobalGroupSpace
   */
  ~GlobalGroupSpace(void)
  {
    reset();
  }

  /**
   *  @brief clears the groups
   */
  void reset(void);

  /**
   *  @brief groups the dataset into groups of equal length
   *    using the metric to determine similarity
   *
   *  @param metric the metric used to group by
   *  @param threshold the threshold to be group with
   *  @return the number of groups it creates
   */
  int group(const std::string& distance_name, data_t threshold);
 
  /**
   *  @brief gets the most similar sequence in the dataset
   *
   *  @param query gets most similar sequence to the query
   *  @return the best match in the dataset
   */
  candidate_time_series_t getBestMatch(const TimeSeries& query);

  /**
   *  @brief find k similar time series to the query
   *
   *  @param query gets most similar sequence to the query
   *  @param k number of similar time series
   *  @param approx if true, return the approximated distance, otherwise return the exact distance
   *  @return the best match in the dataset
   */
  std::vector<candidate_time_series_t> kSim(const TimeSeries& query, int k);
  
  void saveGroups(std::ofstream &fout, bool groupSizeOnly) const;
  int loadGroups(std::ifstream &fin);
  /**
   *  @brief returns true if dataset is grouped
   */
  bool grouped(void) const;

private:

  std::vector<LocalLengthGroupSpace*> localLengthGroupSpace;
  const TimeSeriesSet& dataset;
  dist_t pairwiseDistance;
  dist_t warpedDistance;
  data_t threshold;
  std::string distanceName;

  void loadDistance(const std::string& distanceName);
};

vector<int> generateTraverseOrder(int queryLength, int totalLength);

} // namespace genex
#endif //GLOBAL_GROUP_SPACE_H

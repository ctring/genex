#ifndef GLOBAL_GROUP_SPACE_H
#define GLOBAL_GROUP_SPACE_H

#include <fstream>
#include <vector>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>

#include "group/LocalLengthGroupSpace.hpp"
#include "TimeSeries.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "group/Group.hpp"

namespace genex {

vector<int> generateTraverseOrder(int queryLength, int totalLength);

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
  GlobalGroupSpace(const TimeSeriesSet& dataset);

  /**
   *  @brief The deconstructor for the GlobalGroupSpace
   */
  ~GlobalGroupSpace();

  /**
   *  @brief clears the groups
   */
  void reset();

  /**
   *  @brief groups the dataset into groups of equal length
   *    using the metric to determine similarity
   *
   *  @param metric the metric used to group by
   *  @param threshold the threshold to be group with
   *  @return the number of groups it creates
   */
  int group(const std::string& distance_name, data_t threshold);
  int groupMultiThreaded(const std::string& distance_name, data_t threshold, int num_thread);
  
  int getTotalNumberOfGroups() const;
  std::string getDistanceName() const;
  data_t getThreshold() const;

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
  std::vector<candidate_time_series_t> getKBestMatches(const TimeSeries& query, int k);
  
  void saveGroupsOld(std::ofstream &fout, bool groupSizeOnly) const;
  int loadGroupsOld(std::ifstream &fin);

private:
  std::string distanceName;
  std::vector<LocalLengthGroupSpace*> localLengthGroupSpace;
  const TimeSeriesSet& dataset;
  dist_t pairwiseDistance;
  dist_t warpedDistance;
  data_t threshold;
  int totalNumberOfGroups = 0;

  void _loadDistance(const std::string& distanceName);
  int _group(int i);

  /*************************
   *  Start serialization
   *************************/
  friend class boost::serialization::access;
  template<class A>
  void save(A & ar, unsigned) const
  {
    ar << this->localLengthGroupSpace.size() << this->distanceName << this->threshold;
    for (auto i = 2; i < this->localLengthGroupSpace.size(); i++) {
      ar << *(this->localLengthGroupSpace[i]);
    }
  }

  template<class A>
  void load(A & ar, unsigned)
  {
    size_t maxLen;
    ar >> maxLen >> this->distanceName >> this->threshold;
    this->_loadDistance(this->distanceName);
    this->localLengthGroupSpace.resize(dataset.getItemLength() + 1, nullptr);
    this->totalNumberOfGroups = 0;
    for (auto i = 2; i < maxLen; i++) {
      auto llgs = new LocalLengthGroupSpace(dataset, i);
      ar >> *llgs;
      this->localLengthGroupSpace[i] = llgs;
      this->totalNumberOfGroups += llgs->getNumberOfGroups();
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
  /*************************
   *  End serialization
   *************************/
};

} // namespace genex
#endif //GLOBAL_GROUP_SPACE_H

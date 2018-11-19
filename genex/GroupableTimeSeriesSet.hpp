#ifndef GROUPABLE_TIME_SERIES_SET_H
#define GROUPABLE_TIME_SERIES_SET_H

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>

#include "distance/Distance.hpp"
#include "group/GlobalGroupSpace.hpp"
#include "TimeSeriesSet.hpp"

#define GROUP_FILE_VERSION 1

namespace genex {

/**
 *  @brief a GroupableTimeSeriesSet object is a TimeSeriesSet with grouping capability
 */
class GroupableTimeSeriesSet : public TimeSeriesSet
{
public:

  /**
   *  @brief Destructor
   */
  virtual ~GroupableTimeSeriesSet();

  /**
   *  @brief groups the datset into similarity groups
   *
   *  @param distanceName the distance to use for comparing similarity
   *  @param threshold to use for determing the bound of similarity
   *  @param numThreads number of thread to perform grouping
   *  @param wholeSeriesOnly if set to true group the largest length only
   *
   *  @return the number of groups created
   */
  int groupAllLengths(const std::string& distanceName,
                      data_t threshold,
                      int numThreads,
                      bool wholeSeriesOnly);
  
  /**
   *  @brief deletes and clears the groups
   */
  void reset();

  /**
   *  @brief check if the dataset is grouped
   */
  bool isGrouped() const;

  /**
   *  @brief returns name of the distance used for grouping
   */
  int getTotalNumberOfGroups() const;  
  string getDistanceName() const;
  data_t getThreshold() const;

  void saveGroupsOld(const std::string& path, bool groupSizeOnly) const;
  int loadGroupsOld(const std::string& path);
  
  /**
   * @brief Finds the best matching subsequence in the dataset
   *
   * @param other the timeseries to find the match for
   *
   * @return a struct containing the closest TimeSeries and the distance between them
   * @throws exception if dataset is not grouped
   */
  candidate_time_series_t getBestMatch(const TimeSeries& other) const;

  /**
   * @brief Finds k similar timeseries.
   *
   * @param data the timeseries to find the matches for
   * @param k the number of time series to look for.
   * @param h the number of time series to examine.
   * 
   * @return a vector of struct containing the closest TimeSeries and the distance between them
   * @throws exception if dataset is not grouped
   */
  std::vector<candidate_time_series_t> getKBestMatches(const TimeSeries& data, int k, int h);
  
private:
  GlobalGroupSpace* groupsAllLengthSet = nullptr;

  /*************************
   *  Start serialization
   *************************/
  friend class boost::serialization::access;
  template<class A>
  void save(A & ar, unsigned) const
  {
    if (!this->isGrouped()) {
      throw GenexException("No group found");
    }

    ar << this->getItemCount()
       << this->getMaxLength();

    ar << *(this->groupsAllLengthSet);
  }

  template<class A>
  void load(A & ar, unsigned)
  {
    int grpItemCount, grpItemLength;
    ar >> grpItemCount >> grpItemLength;
    if (grpItemCount != this->getItemCount())
    {
      throw GenexException("Incompatible item count");
    }
    if (grpItemLength != this->getMaxLength())
    {
      throw GenexException("Incompatible item length");
    }
    reset();
    this->groupsAllLengthSet = new GlobalGroupSpace(*this);
    ar >> *(this->groupsAllLengthSet);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
  /*************************
   *  End serialization
   *************************/
};

} // namespace genex

#endif // GROUPABLE_TIME_SERIES_SET_H
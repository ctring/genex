#ifndef GROUPABLE_TIME_SERIES_SET_H
#define GROUPABLE_TIME_SERIES_SET_H

#include "TimeSeriesSet.hpp"

namespace genex {

/**
 *  @brief a GroupableTimeSeriesSet object is a TimeSeriesSet with grouping
 *         functionalities
 */
class GroupableTimeSeriesSet : public TimeSeriesSet
{
public:

  /**
   *  @brief default constructor for GroupableTimeSeriesSet
   *
   *  Create a GroupableTimeSeriesSet object with is an empty string for name
   */
  GroupableTimeSeriesSet() {};

  /**
   *  @brief constructor for GroupableTimeSeriesSet
   *
   *  @param name name of the dataset
   */
  GroupableTimeSeriesSet(const std::string& name)
    : TimeSeriesSet(name) {};

};

} // namespace genex

#endif // GROUPABLE_TIME_SERIES_SET_H
#ifndef GROUPABLE_TIME_SERIES_SET_H
#define GROUPABLE_TIME_SERIES_SET_H

#include "TimeSeriesSet.hpp"
#include "GroupsEqualLengthSet.hpp"

namespace genex {

/**
 *  @brief a GroupableTimeSeriesSet object is a TimeSeriesSet with grouping
 *         functionalities
 */
class GroupableTimeSeriesSet : public TimeSeriesSet
{
public:
    /**
     *  @brief groups the datset into similarity groups
     *
     *  @param metric the metric to use for comparing similarity (distance)
     *  @param threshold to use for determing the bound of similarity
     *
     *  @return the number of groups created
     */
    int groupAllLengths(DistanceMetric* metric, data_t threshold);

    /**
     *  @brief deletes and clears the groups
     */ 
    void resetGrouping(void);

    /**
     *  @brief Calculate the distance between a subsequence of a series in this dataset to
     *   input timeseries
     *
     * @param idx indexs which time series this subsequence belongs to
     * @param start where in the time series the subsequence starts
     * @param length how long the timeseries is
     * @param other the other time series we are comparing against
     * 
     * @return the calculated distance. If the arguments are invalid we throw an exception
     */
    data_t distanceBetween(int idx, int start, int length,
        const TimeSeries& other, DistanceMetric *metric);

    /**
     * @brief Finds the best matching subsequence in the dataset
     *
     * @param the timeseries to find the match for
     * @param the metric to use for finding the distance
     *
     * @return a struct containing the closest TimeSeries and the distance between them
     */
    candidate_time_series_t getBestMatch(const TimeSeries& other, DistanceMetric* metric);

};

} // namespace genex

#endif // GROUPABLE_TIME_SERIES_SET_H
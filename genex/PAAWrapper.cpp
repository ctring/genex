#include "PAAWrapper.hpp"

#include <iostream>
#include <vector>
#include "Exception.hpp"
#include "distance/Distance.hpp"

using std::vector;
using std::min;
using std::cout;
using std::endl;

namespace genex {

PAAWrapper::PAAWrapper(const TimeSeriesSet& dataset): dataset(dataset)
{
  for (int i = 0; i < dataset.getItemCount(); i++) {
    TimeSeries ts = dataset.getTimeSeries(i);
    this->paaMat.push_back(vector<TimeSeries>{ts});
  }
}

/**
 * Perform PAA on a time series.
 */
TimeSeries tsPAA(const TimeSeries& source, int blockSize)
{
  data_t sum = 0;
  int count = 0;
  int srcLength = source.getLength();
  int destLength = (srcLength - 1) / blockSize + 1;
  TimeSeries dest(destLength);
  for (int i = 0; i < srcLength; i++)
  {
    count++;
    sum += source[i];
    if (count == blockSize || i == srcLength - 1) {
      dest[i / blockSize] = sum / count;
      sum = 0;
      count = 0;
    }
  }
  return dest;
}

void PAAWrapper::generatePAAMatrix(int blockSize)
{
  if (blockSize < 1) {
    throw GenexException("Block size for PAA must be positive");
  }

  int itemLength = dataset.getItemLength();
  blockSize = min(blockSize, itemLength);
  for (int i = 0; i < dataset.getItemCount(); i++) {
    this->paaMat[i].clear();
    for (int offset = 0; offset < blockSize; offset++) {
      TimeSeries paaTS = tsPAA(dataset.getTimeSeries(i, offset, itemLength), blockSize);      
      this->paaMat[i].push_back(paaTS);
    }
  }
  this->blockSize = blockSize;
}

TimeSeries PAAWrapper::getPAA(int index, int start, int end) {
    if (index < 0 || index >= dataset.getItemCount())
    {
      throw GenexException("Invalid time series index");
    }
    if (start < 0 && end < 0)
    {
      start = 0;
      end = dataset.getItemLength();
    }
    if (start < 0 || start >= end || end > dataset.getItemLength())
    {
      throw GenexException("Invalid starting or ending position of a time series");
    }
    int offset = start % blockSize;
    TimeSeries &wholePAA = paaMat[index][offset];

    // -1 to shift to 0-based, +1 to shift back to 1-based
    int paaLength = (end - start - 1) / blockSize + 1;
    TimeSeries paaTS{paaLength};
    int paaStart = (start - offset) / blockSize;
    int paaEnd = (end - offset) / blockSize;
    for (int i = paaStart; i < paaEnd; i++) {
      paaTS[i - paaStart] = wholePAA[i];
    }
    // Compute average of the leftover if start and end do not cover the whole cached PAA
    if ((end - offset) % blockSize != 0) {
      int leftOverStart = start + (end - start) / blockSize * blockSize;
      TimeSeries ts = dataset.getTimeSeries(index, leftOverStart, end);
      paaTS[paaLength - 1] = ts.average();
    }
    return paaTS;
  }

vector<candidate_time_series_t> PAAWrapper::getKBestMatchesPAA(
  const TimeSeries& query, int k, const string& distanceName)
{
  if (k <= 0) {
    throw GenexException("K must be positive");
  }

  vector<candidate_time_series_t> bestSoFar;

  auto warpedDistance = getDistanceFromName(distanceName + DTW_SUFFIX);
  data_t bestSoFarDist, currentDist;
  auto timeSeriesLength = dataset.getItemLength();
  auto numberTimeSeries = dataset.getItemCount();
  auto paaQuery = tsPAA(query, this->blockSize);

  // iterate through every timeseries
  for (int idx = 0; idx < numberTimeSeries; idx++)
  {
    // iterate through every length of interval
    for (int intervalLength = 2; intervalLength <= timeSeriesLength;
        intervalLength++) 
    {
      // iterate through all interval window lengths
      for (int start = 0; start <= timeSeriesLength - intervalLength; 
            start++) 
      {
        auto currentTimeSeries = dataset.getTimeSeries(idx, start, start + intervalLength);
        auto currentPAATimeSeries = getPAA(idx, start, start + intervalLength);
        if (k > 0) {
          currentDist = warpedDistance(
            paaQuery, currentPAATimeSeries, INF, gNoMatching);
          bestSoFar.push_back(candidate_time_series_t(currentTimeSeries, currentDist));
          k--;
          if (k == 0) {
            // Heapify exactly once when the heap is filled.
            std::make_heap(bestSoFar.begin(), bestSoFar.end());
          }
        } 
        else
        {
          bestSoFarDist = bestSoFar.front().dist;
          currentDist = warpedDistance(
            paaQuery, currentPAATimeSeries, bestSoFarDist, gNoMatching);
          if (currentDist < bestSoFarDist)
          {
            bestSoFar.push_back(candidate_time_series_t(currentTimeSeries, currentDist));
            std::push_heap(bestSoFar.begin(), bestSoFar.end());
            std::pop_heap(bestSoFar.begin(), bestSoFar.end());
            bestSoFar.pop_back();
          } 
        }
      }
    }
  }

  std::sort(bestSoFar.begin(), bestSoFar.end());

  return bestSoFar;
}

} // namespace genex
#include "TimeSeriesSet.hpp"

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <boost/tokenizer.hpp>

#include "distance/Distance.hpp"
#include "Exception.hpp"

using std::string;
using std::cout;
using std::endl;
using std::vector;

namespace genex {

TimeSeriesSet::TimeSeriesSet()
  : maxCol(0)
  , itemCount(0)
  , normalized(false) {};

TimeSeriesSet::~TimeSeriesSet()
{
  this->clearData();
}

string TimeSeriesSet::getTimeSeriesName(int index) const
{
  if (index < 0 || index >= this->itemCount)
  {
    throw GenexException("Invalid time series index");
  }
  if (index < this->names.size()) 
  {
    return this->names[index];
  }
  return std::to_string(index);
}

std::pair<int, int> _getMaxRowsMaxColumns(std::ifstream& f, const string& separators)
{
  int lineCount = 0;
  int maxColumn = 0;
  int length;
  string line;
  boost::char_separator<char> sep(separators.c_str());
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  for (lineCount = 0; std::getline(f, line); ++lineCount)
  {
    tokenizer tokens(line, sep);
    length = std::distance(tokens.begin(), tokens.end());
    if (maxColumn < length) {
      maxColumn = length;
    }
  }
  f.clear();
  f.seekg(0);
  return {lineCount, maxColumn};
}

void TimeSeriesSet::loadData(const string& filePath
                            , int maxNumRow
                            , int startCol
                            , const string& separators
                            , bool hasNameCol)
{
  this->clearData();

  std::ifstream f(filePath);
  if (!f.is_open())
  {
    f.close();
    throw GenexException(string("Cannot open ") + filePath);
  }
  auto maxRowMaxColumn = _getMaxRowsMaxColumns(f, separators);
  if (maxNumRow <= 0) {
    maxNumRow = maxRowMaxColumn.first;
  }
  else {
    maxNumRow = std::min(maxNumRow, maxRowMaxColumn.first);
  }
  auto rawMaxCol = maxRowMaxColumn.second;
  int length = -1;
  string line;
  // Tokenize each line using the given separators
  boost::char_separator<char> sep(separators.c_str());
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

  this->itemCount = 0;
  this->data = new data_t[maxNumRow * rawMaxCol];
  memset(this->data, 0, maxNumRow * rawMaxCol * sizeof(data_t*));
  for (int row = 0; row < maxNumRow; row++, this->itemCount++)
  {
    // Read and process line by line
    if (getline(f, line))
    {
      tokenizer tokens(line, sep);
      this->lengths.push_back(std::distance(tokens.begin(), tokens.end()) - startCol - hasNameCol);
      int col = 0;
      for (tokenizer::iterator tok_iter = tokens.begin();
           tok_iter != tokens.end(); tok_iter++, col++)
      {
        // Only read columns from startCol and after
        if (col >= startCol)
        {
          try
          {
            if (col == startCol && hasNameCol) {
              this->names.push_back(*tok_iter);
            }
            else {
              int index = row * (rawMaxCol - startCol - hasNameCol) + (col - startCol - hasNameCol);
              this->data[index] = (data_t)std::stod(*tok_iter);
            }
          }
          catch (const std::invalid_argument& e)
          {
            f.close();
            this->clearData();
            throw GenexException("Dataset file contains unparsable text");
          }
          catch (const std::out_of_range& e)
          {
            f.close();
            this->clearData();
            throw GenexException("Values are out of range");
          }
        }
      }
    }
    else {
      break;
    }
  }


  if (f.bad())
  {
    f.close();
    this->clearData();
    throw GenexException("Error while reading file");
  }

  this->filePath = filePath;
  this->maxCol = rawMaxCol - startCol - hasNameCol;
  f.close();
}

void TimeSeriesSet::saveData(const string& filePath, char separator) const
{
  std::ofstream f(filePath);
  if (!f.is_open())
  {
    f.close();
    throw GenexException(string("Cannot open ") + filePath);
  }
  for (int i = 0; i < itemCount; i++) {
    if (!this->names.empty()) {
      f << names[i] << separator;
    }
    for (int j = 0; j < this->lengths[i]; j++) {
      f << data[i * this->maxCol + j] << separator;
    }
    f << endl;
  }
  f.close();
}

void TimeSeriesSet::clearData()
{
  delete[] this->data;
  this->data = nullptr;
  this->itemCount = 0;
  this->maxCol = 0;
  this->names.clear();
}

TimeSeries TimeSeriesSet::getTimeSeries(int index, int start, int end) const
{
  if (index < 0 || index >= this->itemCount)
  {
    throw GenexException("Invalid time series index");
  }
  if (start < 0 && end < 0)
  {
    return TimeSeries(this->data + index * this->maxCol, index, 0, this->lengths[index]);
  }
  if (start < 0 || start >= end || end > this->lengths[index])
  {
    throw GenexException("Invalid starting or ending position of a time series");
  }
  return TimeSeries(this->data + index * this->maxCol, index, start, end);
}

std::pair<data_t, data_t> TimeSeriesSet::normalize(void)
{

  if (!this->itemCount)
  {
    throw GenexException("No data to normalize");
  }

  // auto minAndMax = std::minmax(this->data);
  // data_t diff = *minAndMax.first - *minAndMax.second;
  data_t MIN = INF;
  data_t MAX = -INF;
  unsigned int i;

  // find min and max in 1.5 comparisons per element
  // for (int ts = 0; ts < this->itemCount; ts++)
  // {
  //   data_t x, y, z;

  //   // start at 0 if even, 1 if odd
  //   if ((i = (length % 2 != 0)))
  //   {
  //     if (data[ts * this->itemLength] < MIN)
  //     {
  //       MIN = data[ts * this->itemLength];
  //     }
  //     if (data[ts * this->itemLength] > MAX)
  //     {
  //       MAX = data[ts * this->itemLength];
  //     }
  //   }

  //   for (; i < this->itemLength - 1; i += 2)
  //   {
  //     x = data[ts * this->itemLength + i];
  //     y = data[ts * this->itemLength + (i + 1)];
  //     if ( x > y )
  //     {
  //       z = y;
  //       y = x;
  //       x = z;
  //     }
  //     if ( y > MAX )
  //     {
  //       MAX = y;
  //     }
  //     if ( x < MIN )
  //     {
  //       MIN = x;
  //     }
  //   }
  // }

  for (int ts = 0; ts < this->itemCount; ts++) {
    for (i = 0; i < this->lengths[ts]; i++) {
      if (data[ts * this->maxCol + i] > MAX) {
        MAX = data[ts * this->maxCol + i];
      }

      if (data[ts * this->maxCol + i] < MIN) {
        MIN = data[ts * this->maxCol + i];
      }
    }
  }

  data_t diff = MAX - MIN;

  // do not divide by zero
  if (diff == 0.0)
  {
    if (MAX != 0)
    {
      // zero out data (use memset instead...)
      for (int ts = 0; ts < this->itemCount; ts++)
      {
        for (i = 0; i < this->lengths[ts]; i++)
        {
          data[ts * this->maxCol + i] = 0;
        }
      }
    }
  }
  else
  {
    // normalize
    for (int ts = 0; ts < this->itemCount; ts++)
    {
      for (i = 0; i < this->lengths[ts]; i++)
      {
        data[ts * this->maxCol + i] = (data[ts * this->maxCol + i] - MIN)/ diff;
      }
    }
  }
  normalized = true;
  return std::make_pair(MIN, MAX);
}

vector<candidate_time_series_t> TimeSeriesSet::getKBestMatchesBruteForce(
  const TimeSeries& query, int k, string distanceName)
{
  if (k <= 0) {
    throw GenexException("K must be positive");
  }

  vector<candidate_time_series_t> bestSoFar;

  auto warpedDistance = getDistanceFromName(distanceName + DTW_SUFFIX);
  data_t bestSoFarDist, currentDist;
  // auto timeSeriesLength = getItemLength();
  auto numberTimeSeries = getItemCount();
  
  // iterate through every timeseries
  for (int idx = 0; idx < numberTimeSeries; idx++)
  {
    // iterate through every length of interval
    for (int intervalLength = 2; intervalLength <= getItemLength(idx);
        intervalLength++) 
    {
      // iterate through all interval window lengths
      for (int start = 0; start <= getItemLength(idx) - intervalLength; 
            start++) 
      {
        TimeSeries currentTimeSeries = getTimeSeries(idx, start, start + intervalLength);
        if (k > 0) {
          currentDist = warpedDistance(
            query, currentTimeSeries, INF, gNoMatching);
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
            query, currentTimeSeries, bestSoFarDist, gNoMatching);
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

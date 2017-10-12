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

namespace genex {

TimeSeriesSet::~TimeSeriesSet()
{
  this->clearData();
}

int countNumberOfLines(std::ifstream& f)
{
  int lineCount = 0;
  string line;
  for (lineCount = 0; std::getline(f, line); ++lineCount);
  f.clear();
  f.seekg(0);
  return lineCount;
}

inline int calcPAALength(int srcLength, int n)
{
  return (srcLength - 1) / n + 1;
}

void doPAA(const data_t* source, data_t* dest, int srcLength, int n)
{
  data_t sum = 0;
  int count = 0;
  int destLength = calcPAALength(srcLength, n);
  for (int i = 0; i < srcLength; i++)
  {
    count++;
    sum += source[i];
    if (count == n || i == srcLength - 1) {
      dest[i / n] = sum / count;
      sum = 0;
      count = 0;
    }
  }
}

TimeSeries tsPAA(const TimeSeries& source, int n)
{
  data_t sum = 0;
  int count = 0;
  int srcLength = source.getLength();
  int destLength = calcPAALength(srcLength, n);
  TimeSeries dest(destLength);
  for (int i = 0; i < srcLength; i++)
  {
    count++;
    sum += source[i];
    if (count == n || i == srcLength - 1) {
      dest[i / n] = sum / count;
      sum = 0;
      count = 0;
    }
  }
  return dest;
}

void TimeSeriesSet::loadData(const string& filePath, int maxNumRow,
                             int startCol, const string& separators)
{
  this->clearData();

  std::ifstream f(filePath);
  if (!f.is_open())
  {
    f.close();
    throw GenexException(string("Cannot open ") + filePath);
  }

  if (maxNumRow <= 0) {
    maxNumRow = countNumberOfLines(f);
  }
  else {
    maxNumRow = std::min(maxNumRow, countNumberOfLines(f));
  }

  int length = -1;
  string line;
  boost::char_separator<char> sep(separators.c_str());
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

  this->itemCount = 0;
  for (int row = 0; row < maxNumRow; row++, this->itemCount++)
  {
    // Read and process line by line
    if (getline(f, line))
    {
      tokenizer tokens(line, sep);

      // Number of columns in the first line is assumed to be number of columns of
      // the whole dataset
      if (row == 0)
      {
        // If this is the first row, set length of each row to length of this row
        length = std::distance(tokens.begin(), tokens.end());
        this->data = new data_t[maxNumRow * length];
        memset(this->data, 0, maxNumRow * length * sizeof(data_t*));

      }
      else if (length != std::distance(tokens.begin(), tokens.end()))
      {
        f.close();
        this->clearData();
        throw GenexException("File contains time series with inconsistent lengths");
      }

      int col = 0;
      for (tokenizer::iterator tok_iter = tokens.begin();
           tok_iter != tokens.end(); tok_iter++, col++)
      {
        // Only read columns from startCol and after
        if (col >= startCol)
        {
          try
          {
            data[row * length + (col - startCol)] = (data_t)std::stod(*tok_iter);
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

  this->itemLength = length - startCol;
  this->filePath = filePath;

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
    for (int j = 0; j < itemLength; j++) {
      f << data[i * itemLength + j] << separator;
    }
    f << std::endl;
  }
  f.close();
}

void TimeSeriesSet::clearData()
{
  delete[] this->data;
  this->data = nullptr;
  this->itemCount = 0;
  this->itemLength = 0;
}

TimeSeries TimeSeriesSet::getTimeSeries(int index, int start, int end) const
{
  if (index < 0 || index >= this->itemCount)
  {
    throw GenexException("Invalid time series index");
  }
  if (start < 0 && end < 0)
  {
    return TimeSeries(this->data + index * this->itemLength, index, 0, this->itemLength);
  }
  if (start < 0 || start >= end || end > this->itemLength)
  {
    throw GenexException("Invalid starting or ending position of a time series");
  }
  return TimeSeries(this->data + index * this->itemLength, index, start, end);
}

std::pair<data_t, data_t> TimeSeriesSet::normalize(void)
{
  int length = this->getItemLength() * this->getItemCount();

  if (!length)
  {
    throw GenexException("No data to normalize");
  }

  // auto minAndMax = std::minmax(this->data);
  // data_t diff = *minAndMax.first - *minAndMax.second;
  data_t MIN = INF;
  data_t MAX = -INF;
  unsigned int i;

  // find min and max in 1.5 comparisons per element
  for (int ts = 0; ts < this->itemCount; ts++)
  {
    data_t x, y, z;

    // start at 0 if even, 1 if odd
    if ((i = (length % 2 != 0)))
    {
      if (data[ts * this->itemLength] < MIN)
      {
        MIN = data[ts * this->itemLength];
      }
      if (data[ts * this->itemLength] > MAX)
      {
        MAX = data[ts * this->itemLength];
      }
    }

    for (; i < this->itemLength - 1; i += 2)
    {
      x = data[ts * this->itemLength + i];
      y = data[ts * this->itemLength + (i + 1)];
      if ( x > y )
      {
        z = y;
        y = x;
        x = z;
      }
      if ( y > MAX )
      {
        MAX = y;
      }
      if ( x < MIN )
      {
        MIN = x;
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
        for (i = 0; i < this->itemLength; i++)
        {
          data[ts * this->itemLength + i] = 0;
        }
      }
    }
  }
  else
  {
    // normalize
    for (int ts = 0; ts < this->itemCount; ts++)
    {
      for (i = 0; i < this->itemLength; i++)
      {
        data[ts * this->itemLength + i] = (data[ts * this->itemLength + i] - MIN)/ diff;
      }
    }
  }
  normalized = true;
  return std::make_pair(MIN, MAX);
}

void TimeSeriesSet::PAA(int n)
{
  if (n <= 0) {
    throw GenexException("Block size must be positive");
  }
  int newItemLength = calcPAALength(this->itemLength, n);
  data_t* new_data = new data_t[this->itemCount * newItemLength];
  for (int ts = 0; ts < this->itemCount; ts++)
  {
    doPAA(this->data + ts * this->itemLength, new_data + ts * newItemLength,
      this->itemLength, n);
  }
  delete this->data;
  this->data = new_data;
  this->itemLength = newItemLength;
}

bool TimeSeriesSet::isLoaded()
{
  return this->data != nullptr;
}

std::vector<candidate_time_series_t> TimeSeriesSet::kSimRaw(
  const TimeSeries& query, int k, int PAABlock)
{
  if (k <= 0) {
    throw GenexException("K must be positive");
  }
  std::vector<candidate_time_series_t> bestSoFar;
  const std::string& distance_name = "euclidean";
  dist_t warpedDistance = getDistance(distance_name + DTW_SUFFIX);
  data_t bestSoFarDist, currentDist;
  int timeSeriesLength = getItemLength();
  int numberTimeSeries = getItemCount();
  
  TimeSeries PAAQuery(0);
  if (PAABlock > 0)
  {
    PAAQuery = tsPAA(query, PAABlock);
  }

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
        TimeSeries currentTimeSeries = getTimeSeries(idx, start, start + intervalLength);
        if (k > 0) {
          if (PAABlock > 0)
          {
            TimeSeries PAACurrentTimeSeries = tsPAA(currentTimeSeries, PAABlock);
            currentDist = warpedDistance(PAAQuery, PAACurrentTimeSeries, INF);
          }
          else {
            currentDist = warpedDistance(query, currentTimeSeries, INF);
          }
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
          if (PAABlock > 0)
          {
            TimeSeries PAACurrentTimeSeries = tsPAA(currentTimeSeries, PAABlock);
            currentDist = warpedDistance(PAAQuery, PAACurrentTimeSeries, bestSoFarDist);
          }
          else {
            currentDist = warpedDistance(query, currentTimeSeries, bestSoFarDist);
          }
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

  if (PAABlock > 0) {
    for (unsigned int i = 0; i < bestSoFar.size(); i++) {
      bestSoFar[i].dist = warpedDistance(query, bestSoFar[i].data, INF);
    }
  }
  std::sort(bestSoFar.begin(), bestSoFar.end());
  
  return bestSoFar;
}

} // namespace genex

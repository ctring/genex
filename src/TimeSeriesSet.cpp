#include "TimeSeriesSet.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <boost/tokenizer.hpp>

#include "Exception.hpp"

namespace genex {

TimeSeriesSet::~TimeSeriesSet()
{
  this->clearData();
}

void TimeSeriesSet::loadData(const std::string& filePath, int maxNumRow,
                             const std::string& separators, int startCol)
{
  this->clearData();

  std::ifstream f(filePath.c_str());
  if (!f.is_open())
  {
    f.close();
    throw GenexException("Cannot open file"); // TODO: use strerror(errno)
  }

  if (maxNumRow <= 0) { maxNumRow = 999999999; }

  int length = -1;
  std::string line;
  boost::char_separator<char> sep(separators.c_str());
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

  this->itemCount = 0;
  for (int row = 0; row < maxNumRow; row++, this->itemCount++)
  {
    if (getline(f, line))
    {
      tokenizer tokens(line, sep);

      // Number of columns in the first line is assumed to be number of columns of
      // the whole dataset
      if (row == 0)
      {
        // If this is the first row, set length of each row to length of this row
        length = std::distance(tokens.begin(), tokens.end());
      }
      else if (length != std::distance(tokens.begin(), tokens.end()))
      {
        // If some row has a different length, throw an error
        f.close();
        this->clearData();
        throw GenexException("File contains time series with inconsistent lengths");
      }

      data.push_back(new data_t[length - startCol]);
      int col = 0;
      for (tokenizer::iterator tok_iter = tokens.begin();
           tok_iter != tokens.end(); tok_iter++, col++)
      {
        if (col >= startCol)
        {
          data[row][col - startCol] = (data_t)std::stod(*tok_iter);
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

void TimeSeriesSet::clearData()
{
  for (int i = 0; i < this->itemCount; i++)
  {
    delete[] this->data[i];
    this->data[i] = NULL;
  }
  this->data.clear();
  this->itemCount = 0;
  this->itemLength = 0;
}

TimeSeries TimeSeriesSet::getTimeSeries(int index, int start, int end) const
{
  if (index < 0 || index >= this->itemCount)
  {
    throw GenexException("Invalid time series index");
  }
  if (start < 0 || start >= end || end > this->itemLength)
  {
    throw GenexException("Invalid starting or ending position of a time series");
  }

  return TimeSeries(this->data[index], index, start, end);
}

TimeSeries TimeSeriesSet::getTimeSeries(int index) const
{
  return this->getTimeSeries(index, 0, this->itemLength);
}

} // namespace genex
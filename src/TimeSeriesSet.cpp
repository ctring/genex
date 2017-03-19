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

  this->_resizeData(maxNumRow);

  int length = -1;
  std::string line;
  boost::char_separator<char> sep(separators.c_str());
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

  for (int row = 0; row < maxNumRow; row++)
  {
    if (getline(f, line))
    {
      tokenizer tokens(line, sep);

      // Number of columns in the first line is assumed to be number of columns of
      // the whole dataset
      if (length < 0)
      {
        length = std::distance(tokens.begin(), tokens.end());
      }
      else if (length != std::distance(tokens.begin(), tokens.end()))
      {
        f.close();
        this->clearData();
        throw GenexException("File contains time series with inconsistent lengths");
      }

      data[row] = new data_t[length];
      int col = 0;
      for (tokenizer::iterator tok_iter = tokens.begin();
           tok_iter != tokens.end(); tok_iter++, col++)
      {
        data[row][col] = (data_t)std::stod(*tok_iter);
      }
    }
    else {
      // Number of rows in the file is smaller than maxNumRow, resize the data to
      // fix the real row count
      this->_resizeData(row);
      break;
    }
  }

  if (f.bad())
  {
    f.close();
    this->clearData();
    throw GenexException("Error while reading file");
  }
  this->itemLength = length;
  f.close();
}

void TimeSeriesSet::clearData()
{
  for (int i = 0; i < this->itemCount; i++)
  {
    delete[] this->data[i];
    this->data[i] = NULL;
  }
  delete[] this->data;
  this->data = NULL;
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

  return TimeSeries(this->data[index] + start, index, start, end);
}

TimeSeries TimeSeriesSet::getTimeSeries(int index) const
{
  return this->getTimeSeries(index, 0, this->itemLength);
}

void TimeSeriesSet::_resizeData(int size)
{
  data_t** temp = new data_t*[size];
  memset(temp, 0, size * sizeof(data_t*));

  for (int i = 0; i < std::min(size, this->itemCount); i++)
  {
    temp[i] = this->data[i];
  }

  for (int i = size; i < this->itemCount; i++)
  {
    delete[] this->data[i];
  }
  delete[] this->data;

  this->data = temp;
  this->itemCount = size;
}

} // namespace genex
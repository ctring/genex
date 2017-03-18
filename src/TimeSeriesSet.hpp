#ifndef TIMESERIESSET_H
#define TIMESERIESSET_H

#include <string>

#include "TimeSeries.hpp"

class TimeSeriesSet
{
public:
  TimeSeriesSet()
    : name(""), data(NULL), itemLength(0), itemCount(0){};
  TimeSeriesSet(std::string name)
    : name(name), data(NULL), itemLength(0), itemCount(0) {};
  ~TimeSeriesSet();

  void loadData(std::string filePath, int maxNumRow, std::string separator = " ", int startCol = 0);
  void clearData();

  int getItemLength() const { return itemLength; }
  int getItemCount() const { return itemCount; }
  const std::string& getName() const { return name; }

  TimeSeries getTimeSeries(int index);
  TimeSeries getTimeSeries(int index, int start, int end);

private:
  void _resizeData(int size);
  data_t** data;
  int itemLength;
  int itemCount;
  const std::string name;

};

#endif // TIMESERIESSET_H
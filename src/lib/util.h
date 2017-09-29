#ifndef UTIL_H
#define UTIL_H

#include "TimeSeries.hpp"

using namespace genex;

/// Data structure for sorting the query
class Index
{
 public:
    data_t value;
    int    index;

    int diff(Index &other);
    static int comp(const void *a, const void *b);
  
};

void printArray(data_t *x, int len);
void error(int id);


#endif // UTIL_H

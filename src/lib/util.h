#ifndef UTIL_H
#define UTIL_H

/// Data structure for sorting the query
class Index
{
 public:
    double value;
    int    index;

    int diff(Index &other);
    static int comp(const void *a, const void *b);
  
};

void printArray(double *x, int len);
void error(int id);


#endif // UTIL_H

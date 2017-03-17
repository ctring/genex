#ifndef GENEX_SRC_DISTANCE_MANAGER_H
#define GENEX_SRC_DISTANCE_MANAGER_H

#include "DistanceMetric.hpp"

/**
 *  @brief manages and returns correct metric given a correct string descriptor
 *
 *  Example:
 *    DistanceManager m;
 *    DistanceMetric * euc = m.getMetric("euclidean");
 *    cout << euc.dist(A,B) << endl;
 */
class DistanceManager
{
public:
  DistanceMetric * getMetric(const std::string d);
};

#endif // GENEX_SRC_DISTANCE_MANAGER_H

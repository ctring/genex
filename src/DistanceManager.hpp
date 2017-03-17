#ifndef GENEX_SRC_DISTANCE_MANAGER_H
#define GENEX_SRC_DISTANCE_MANAGER_H

#include "DistanceMetric.hpp"
#include "Exception.hpp"

//Add additional distance descriptors here
enum Dists { EUC, MINK, USER_DEF_1 };

/**
 *  @brief manages and returns correct metric given a Dist (ENUM)
 *
 *  Example:
 *    DistanceManager m;
 *    DistanceMetric * euc = m.getMetric(Dists::EUC);
 *    cout << euc.dist(A,B) << endl;
 */
class DistanceManager
{
public:
  DistanceMetric * getMetric(Dists d);
};

#endif // GENEX_SRC_DISTANCE_MANAGER_H

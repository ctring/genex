#ifndef GENEX_SRC_DISTANCE_MANAGER_H
#define GENEX_SRC_DISTANCE_MANAGER_H

#include "DistanceMetric.hpp"

/**
 *  @brief this class returns metric given a string descriptor
 *
 *  Example:
 *    DistanceManager m;
 *    DistanceMetric * euc = m.getMetric("euclidean");
 *    cout << euc.dist(A,B) << endl;
 */
class DistanceManager
{
public:

  static DistanceManager* getInstance()
  {
    if (!instance)
    {
      instance = new DistanceManager;
    }
    return instance;
  }

  DistanceMetric* getMetric(const std::string d);

private:
  static DistanceManager* instance;

  DistanceManager() { }
};

#endif // GENEX_SRC_DISTANCE_MANAGER_H

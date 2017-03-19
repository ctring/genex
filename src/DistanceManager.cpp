#include <iostream>

#include "DistanceManager.hpp"
#include "Exception.hpp"
#include "Euclidean.hpp"
#include "Manhattan.hpp"
#include "Chebyshev.hpp"

DistanceManager* DistanceManager::instance = NULL;

/**
 *  @brief returns a distance metric given a correct string
 *
 *  To extend this a user has to add an if statement
 *  for their given distance function.
 *
 *  Example:
 *  // create new distance called Magic, and add MAGIC to Dists
 *
 *  //in DistanceManager.cpp (below)
 *  else if (d == "magic") {
 *    metric = new Magic;
 *  }
 *
 *  //usage:
 *  DistanceManager m;
 *  DistanceMetric * magic = m.getMetric("magic");
 *
 *  @param d is a string, used to indicate the metric class desired
 *  @return metric is DistanceMetric of the corresponding type
 */
DistanceMetric* DistanceManager::getMetric(const std::string d) const
{
  DistanceMetric * metric = NULL;

  if (d == "euclidean") {
    metric = new Euclidean;
  } else if (d == "manhattan") {
    metric = new Manhattan;
  } else if (d == "chebyshev") {
    metric = new Chebyshev;
  } else {
    throw GenexException("Undefined distance metric");
  }

  return metric;
};

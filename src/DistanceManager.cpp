#include <iostream>
#include "DistanceManager.hpp"
#include "Euclidean.hpp"
#include "Minkowski.hpp"



/**
 *  @brief returns a distance metric given a dist (ENUM)
 *
 *  To extend this a user has to add a case statement
 *  for their given distance function, additionally adding
 *  an identifier to the Dists enum.
 *
 *  Example:
 *  // create new distance called Magic, and add MAGIC to Dists
 *  enum Dists = {... ,MAGIC};
 *
 *  //in DistanceManager
 *  case Dists::MAGIC:
 *    metric = new Magic;
 *    break;
 *
 *  //usage:
 *  DistanceManager m;
 *  DistanceMetric * magic = m.getMetric(Dists::MAGIC);
 *
 *
 *  @param d is a Dists, used to indicate the metric class desired
 *  @return metric is DistanceMetric of the corresponding type
 */
DistanceMetric* DistanceManager::getMetric(Dists d)
{
  DistanceMetric * metric = NULL;

  switch(d) {
   case Dists::EUC:
      metric = new Euclidean;
      break;
    case Dists::MINK:
      metric = new Minkowski;
      break;
    case Dists::USER_DEF_1:
      metric = new Euclidean;
      break;
    default:
      throw GenexException("Undefined distance metric");
  }

  return metric;
};

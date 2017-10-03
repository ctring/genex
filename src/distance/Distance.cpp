#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "Exception.hpp"

////////////////////////////////////////////////////////////////////////////////
/////**********      Include distance headers directly below     **********/////
////////////////////////////////////////////////////////////////////////////////
#include "distance/Euclidean.hpp"
#include "distance/Manhattan.hpp"
#include "distance/Chebyshev.hpp"
#include "distance/Cosine.hpp"
#include "distance/Distance.hpp"
#include "distance/Sorensen.hpp"

namespace genex {

/**
 *  Add distances to this list
 */
static std::vector<const dist_t> gAllDistance =
  {
    NEW_DISTANCE(Euclidean, data_t),
    NEW_DISTANCE(Manhattan, data_t),
    NEW_DISTANCE(Chebyshev, data_t),
    NEW_DISTANCE(Cosine, data_t*),
    NEW_DISTANCE(Sorensen, data_t*)
  };

/**
 *  Add names to this list corresponding to the distances in the above list
 */
static std::vector<std::string> gAllDistanceName =
  {
    NEW_DISTANCE_NAME(euclidean),
    NEW_DISTANCE_NAME(manhattan),
    NEW_DISTANCE_NAME(chebyshev),
    NEW_DISTANCE_NAME(cosine),
    NEW_DISTANCE_NAME(sorensen)
  };

////////////////////////////////////////////////////////////////////////////////
/////**********          no need to make changes below!          **********/////
////////////////////////////////////////////////////////////////////////////////

static std::map<std::string, dist_t> gAllDistanceMap;

void _initializeAllDistanceMap()
{
  if (gAllDistanceMap.empty())
  {
    for (unsigned int i = 0; i < gAllDistance.size(); i++)
    {
      gAllDistanceMap[gAllDistanceName[i]] = gAllDistance[i];
    }
  }
}

const dist_t getDistance(const std::string& distance_name)
{
  _initializeAllDistanceMap();
  if (gAllDistanceMap.find(distance_name) == gAllDistanceMap.end())
  {
    throw GenexException(std::string("Cannot find distance with name: ") + distance_name);
  }
  return gAllDistanceMap[distance_name];
}

const std::vector<std::string>& getAllDistanceName()
{
  return gAllDistanceName;
}

double warpingBandRatio = 0.5;

void setWarpingBandRatio(double ratio) {
  warpingBandRatio = ratio;
}

data_t keoghLowerBound(TimeSeries& a, TimeSeries& b, data_t dropout)
{
  Euclidean e;
  int len = std::min(a.getLength(), b.getLength());
  const data_t* aLower = a.getKeoghLower(warpingBandRatio);
  const data_t* aUpper = a.getKeoghUpper(warpingBandRatio);
  data_t lb = 0;
  data_t idropout = e.inverseNorm(dropout, a, b);

  for (int i = 0; i < len && lb < idropout; i++)
  {
    if (b[i] > aUpper[i]) {
      lb += e.dist(b[i], aUpper[i]);
    }
    else if(b[i] < aLower[i]) {
      lb += e.dist(b[i], aLower[i]);
    }
  }
  return e.norm(lb, a, b);
}

data_t cascadeDistance(TimeSeries& a, TimeSeries& b, data_t dropout)
{

  return 0;
}

} // namespace genex

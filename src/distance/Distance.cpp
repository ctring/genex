#include <map>
#include <vector>

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

#include <iostream>

namespace genex {

/**
 *  Add new distances to this list of pairwise distance
 */
static std::vector<const dist_t> gAllDistance =
  {
    pairwiseDistance<Euclidean, data_t>,
    warpedDistance<Euclidean, data_t>,

    pairwiseDistance<Manhattan, data_t>,
    warpedDistance<Manhattan, data_t>,

    pairwiseDistance<Chebyshev, data_t>,
    warpedDistance<Chebyshev, data_t>,

    pairwiseDistance<Cosine, data_t*>,
    warpedDistance<Cosine, data_t*>,

    pairwiseDistance<Sorensen, data_t*>,
    warpedDistance<Sorensen, data_t*>

  };

static std::vector<std::string> gAllDistanceName =
  {
    "euclidean",
    "euclidean_warp",

    "manhattan",
    "manhattan_warp",

    "chebyshev",
    "chebyshev_warp",

    "cosine",
    "cosine_warp",

    "sorensen",
    "sorensen_warp"
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

} // namespace genex

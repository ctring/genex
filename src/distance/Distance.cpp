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

using std::string;
using std::vector;
using std::min;
using std::max;
using std::cout;
using std::endl;

namespace genex {

/**
 *  Add distances to this list
 */
static vector<dist_t> gAllDistance =
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
static vector<string> gAllDistanceName =
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

static std::map<string, dist_t> gAllDistanceMap;

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

const dist_t getDistance(const string& distance_name)
{
  _initializeAllDistanceMap();
  if (gAllDistanceMap.find(distance_name) == gAllDistanceMap.end())
  {
    throw GenexException(string("Cannot find distance with name: ") + distance_name);
  }
  return gAllDistanceMap[distance_name];
}

const vector<string>& getAllDistanceName()
{
  return gAllDistanceName;
}

double warpingBandRatio = 0.1;

void setWarpingBandRatio(double ratio) {
  warpingBandRatio = ratio;
}

int calculateWarpingBandSize(int length)
{
  int bandSize = floor(length * warpingBandRatio);
  return std::min(bandSize, length - 1);
}

Euclidean _euc;

data_t kimLowerBound(const TimeSeries& a, const TimeSeries& b, data_t dropout)
{
  int al = a.getLength();
  int bl = b.getLength();
  int l = min(al, bl);

  if (l == 0) {
    return 0;
  }

  if (l == 1) {
    return _euc.dist(a[0], b[0]);
  }

  double lb = 0;

  lb += _euc.dist(a[0], b[0]);
  lb += _euc.dist(a[al - 1], b[bl - 1]);
  if (lb > dropout) {
    return INF;
  }

  lb += min(min(_euc.dist(a[0], b[1]),
                _euc.dist(a[1], b[1])),
                _euc.dist(a[1], b[0]));
  if (lb > dropout) {
    return INF;
  }

  lb += min(min(_euc.dist(a[al-1], b[bl-2]),
                _euc.dist(a[al-2], b[bl-2])),
                _euc.dist(a[al-2], b[bl-1]));
  if (lb >=dropout) {
    return INF;
  }

  if (l == 4) {
    return lb;
  }

  lb += min(min(min(_euc.dist(a[0], b[2]),
                    _euc.dist(a[1], b[2])),
                min(_euc.dist(a[2], b[2]),
                    _euc.dist(a[2], b[1]))),
            _euc.dist(a[2], b[0]));

  if (lb > dropout) {
    return INF;
  }

  lb += min(min(min(_euc.dist(a[al-1], b[bl-3]),
                    _euc.dist(a[al-2], b[bl-3])),
                min(_euc.dist(a[al-3], b[bl-3]),
                    _euc.dist(a[al-3], b[bl-2]))),
            _euc.dist(a[al-3], b[bl-1]));
  return lb;
}

data_t keoghLowerBound(const TimeSeries& a, const TimeSeries& b, data_t dropout)
{

  int len = min(a.getLength(), b.getLength());
  int warpingBand = calculateWarpingBandSize(max(a.getLength(), b.getLength()));
  const data_t* aLower = a.getKeoghLower(warpingBand);
  const data_t* aUpper = a.getKeoghUpper(warpingBand);
  data_t idropout = dropout * 2 * max(a.getLength(), b.getLength());
  idropout *= idropout;
  data_t lb = 0;

  for (int i = 0; i < len && lb < idropout; i++)
  {
    if (b[i] > aUpper[i]) {
      lb += _euc.dist(b[i], aUpper[i]);
    }
    else if(b[i] < aLower[i]) {
      lb += _euc.dist(b[i], aLower[i]);
    }
  }
  return _euc.normDTW(lb, a, b);
}

data_t crossKeoghLowerBound(const TimeSeries& a, const TimeSeries& b, data_t dropout)
{
  data_t lb = keoghLowerBound(a, b, dropout);
  if (lb > dropout) {
    return INF;
  }
  else {
    return max(lb, keoghLowerBound(b, a, dropout));
  }
}

data_t cascadeDistance(const TimeSeries& a, const TimeSeries& b, data_t dropout)
{
  // Temporarily disable this because the code seems to be problematic
  // data_t lb = kimLowerBound(a, b, dropout);
  // if (lb > dropout) {
  //   return INF;
  // }
  data_t lb = crossKeoghLowerBound(a, b, dropout);
  if (lb > dropout) {
    return INF;
  }
  data_t d = warpedDistance<Euclidean, data_t>(a, b, dropout);
  return d;
}

} // namespace genex

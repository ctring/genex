#ifndef GENERAL_DISTANCE_H
#define GENERAL_DISTANCE_H

#include <vector>
#include <cmath>
#include "TimeSeries.hpp"
#include "Exception.hpp"

#define NEW_DISTANCE(_class, _type) \
  pairwiseDistance<_class, _type>,  \
  warpedDistance<_class, _type>

#define NEW_DISTANCE_NAME(_name) #_name, #_name"_warp"

namespace genex {

typedef data_t (*dist_t)(const TimeSeries&, const TimeSeries&, data_t);

/**
 *  @brief returns the an object representing a distance metric
 *
 *  @param distance_name name of a distance metric
 *  @return an object containing methods of the requested distance metric
 *  @throw GenexException if no distance with given name is found
 */
const dist_t getDistance(const std::string& distance_name);

/**
 *  @return a vector of names of available distances
 */
const std::vector<std::string>& getAllDistanceName();

template <class T> struct hasInverseNorm
{
  template <typename U, U> struct helper{};
  template <typename C>
  static constexpr decltype(helper<data_t (C::*)(data_t, const TimeSeries&, const TimeSeries&) const,
                                   &C::inverseNorm>(),
                            bool()) test(int /* unused */)
  {
      return true;
  }

  template <typename C> static constexpr bool test(...)
  {
      return false;
  }

  // int is used to give the precedence!
  static constexpr bool value = test<T>(int());
};

/**
 *  @brief returns the warped distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param a one of the two arrays of data
 *  @param b the other of the two arrays of data
 *  @param dropout drops the calculation of distance if within this
 */
template<typename DM, typename T>
data_t warpedDistance(const TimeSeries& a, const TimeSeries& b, data_t dropout)
{
  int m = a.getLength();
  int n = b.getLength();

  static DM* metric = new DM();

  // Fastpath for base intervals.
  if (m == 1 && n == 1)
  {
    T result = metric->init();
    result = metric->reduce(result, result, a[0], b[0]);
    data_t normalizedResult = metric->norm(result, a, b);
    metric->clean(result);
    return normalizedResult;
  }

  // create cost matrix
  std::vector< std::vector< T >> cost(m, std::vector< T >(n));
  std::vector< std::vector< data_t >> ncost(m, std::vector<data_t>(n));

  #if 0
  auto trace = new std::pair<data_t, data_t>*[m]; // For tracing warping
  for (int i = 0; i < m; i++)
  {
      trace[i] = new std::pair<data_t, data_t>[n];
  }
  #endif

  cost[0][0] = metric->init();
  cost[0][0] = metric->reduce(cost[0][0], cost[0][0], a[0], b[0]);
  ncost[0][0] = metric->norm(cost[0][0], a, b);

  #if 0
  trace[0][0] = std::make_pair(-1, -1);
  #endif

  // calculate first column
  for(int i = 1; i < m; i++)
  {
    cost[i][0] = metric->init();
    cost[i][0] = metric->reduce(cost[i][0], cost[i-1][0], a[i], b[0]);
    ncost[i][0] = metric->norm(cost[i][0], a, b);
    #if 0
    trace[i][0] = std::make_pair(i - 1, 0);
    #endif
  }

  // calculate first row
  for(int j = 1; j < n; j++)
  {
    cost[0][j] = metric->init();
    cost[0][j] = metric->reduce(cost[0][j], cost[0][j-1], a[0], b[j]);
    ncost[0][j] = metric->norm(cost[0][j], a, b);
    #if 0
    trace[0][j] = std::make_pair(0, j - 1);
    #endif
  }

  data_t result;
  bool dropped = false;

  for(int i = 1; i < m; i++)
  {
    data_t bestSoFar = ncost[i][0];
    for(int j = 1; j < n; j++)
    {
      T minPrev = cost[i-1][j];
      if (ncost[i-1][j-1] < ncost[i][j-1] && ncost[i-1][j-1] < ncost[i-1][j])
      {
        minPrev = cost[i-1][j-1];
      }
      else if (ncost[i][j-1] < ncost[i-1][j])
      {
        minPrev = cost[i][j-1];
      }
      cost[i][j] = metric->init();
      cost[i][j] = metric->reduce(cost[i][j], minPrev, a[i], b[j]);
      ncost[i][j] = metric->norm(cost[i][j], a, b);
      bestSoFar = std::min(bestSoFar, ncost[i][j]);
    }

    if (bestSoFar > dropout)
    {
      dropped = true;
      break;
    }
  }

  if (dropped)
  {
    result = INF;
  }
  else
  {
    result = ncost[m - 1][n - 1];
  }

  for(int i = 1; i < m; i++)
  {
    for(int j = 1; j < n; j++)
    {
      metric->clean(cost[i][j]);
    }
  }

  return result;
}

/**
 *  @brief returns the distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param x_1 one of the time series
 *  @param x_2 the other of the time series
 *  @param x_3 the length of the data
 */
template<typename DM, typename T>
typename std::enable_if<!hasInverseNorm<DM>::value, data_t>::type
pairwiseDistance(const TimeSeries& x_1, const TimeSeries& x_2, data_t dropout)
{
  if (x_1.getLength() != x_2.getLength())
  {
    throw GenexException("Two time series must have the same length for pairwise distance");
  }

  static DM* metric = nullptr;
  if (metric == nullptr)
  {
    metric = new DM();
  }

  T total = metric->init();

  bool dropped = false;

  for(int i = 0; i < x_1.getLength(); i++)
  {
    total = metric->reduce(total, total, x_1[i], x_2[i]);
    if (metric->norm(total, x_1, x_2) >= dropout)
    {
      dropped = true;
      break;
    }
  }

  data_t result = dropped ? INF : metric->norm(total, x_1, x_2);
  metric->clean(total);

  return result;
}

template<typename DM, typename T>
typename std::enable_if<hasInverseNorm<DM>::value, data_t>::type
pairwiseDistance(const TimeSeries& x_1, const TimeSeries& x_2, data_t dropout)
{
  if (x_1.getLength() != x_2.getLength())
  {
    throw GenexException("Two time series must have the same length for pairwise distance");
  }

  static DM* metric = nullptr;
  if (metric == nullptr)
  {
    metric = new DM();
  }

  T total = metric->init();

  bool dropped = false;
  dropout = metric->inverseNorm(dropout, x_1, x_2);

  for(int i = 0; i < x_1.getLength(); i++)
  {
    total = metric->reduce(total, total, x_1[i], x_2[i]);
    if (total >= dropout)
    {
      dropped = true;
      break;
    }
  }

  data_t result = dropped ? INF : metric->norm(total, x_1, x_2);
  metric->clean(total);

  return result;
}

} // namespace genex

#endif //GENERAL_DISTANCE_H

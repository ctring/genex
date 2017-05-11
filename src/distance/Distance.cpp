#include <map>
#include <vector>

#include "Exception.hpp"
#include "distance/DistanceMetric.hpp"

////////////////////////////////////////////////////////////////////////////////
/////**********      Include distance headers directly below     **********/////
////////////////////////////////////////////////////////////////////////////////
#include "distance/Euclidean.hpp"
#include "distance/Manhattan.hpp"
#include "distance/Chebyshev.hpp"
#include "distance/Cosine.hpp"

#include <iostream>

namespace genex {
namespace distance {

/**
 *  Add new distances to this list of metrics
 */
static std::vector<const DistanceMetric*> gAllMetric =
  {
    new Euclidean(),
    new Manhattan(),
    new Chebyshev(),
    new Cosine()
  };

////////////////////////////////////////////////////////////////////////////////
/////**********          no need to make changes below!          **********/////
////////////////////////////////////////////////////////////////////////////////
static std::map<std::string, const DistanceMetric*> gAllMetricMap;

void _initializeAllMetricMap()
{
  if (gAllMetricMap.empty())
  {
    for (auto metric : gAllMetric)
    {
      gAllMetricMap[metric->getName()] = metric;
    }
  }
}

const DistanceMetric* getDistanceMetric(const std::string& metric_name)
{
  _initializeAllMetricMap();
  if (gAllMetricMap.find(metric_name) == gAllMetricMap.end())
  {
    throw GenexException(std::string("Cannot find distance with name: ") + metric_name);
  }
  return gAllMetricMap[metric_name];
}

const std::vector<const DistanceMetric*>& getAllDistanceMetrics()
{
  return gAllMetric;
}

data_t generalWarpedDistance(const DistanceMetric* metric,
                             const TimeSeries& a,
                             const TimeSeries& b,
                             data_t dropout)
{
  int m = a.getLength();
  int n = b.getLength();

  // Fastpath for base intervals.
  if (m == 1 && n == 1)
  {
    data_t* result = metric->init();
    metric->reduce(result, result, a[0], b[0]);
    data_t normalizedResult = metric->norm(result, a, b);
    delete result;
    return normalizedResult;
  }

  // create cost matrix
  std::vector< std::vector< data_t* >> cost(m, std::vector<data_t*>(n));
  std::vector< std::vector< data_t >> ncost(m, std::vector<data_t>(n));

  #if 0
  auto trace = new std::pair<data_t, data_t>*[m]; // For tracing warping
  for (int i = 0; i < m; i++)
  {
      trace[i] = new std::pair<data_t, data_t>[n];
  }
  #endif

  cost[0][0] = metric->init();
  metric->reduce(cost[0][0], cost[0][0], a[0], b[0]);
  ncost[0][0] = metric->norm(cost[0][0], a, b);

  #if 0
  trace[0][0] = std::make_pair(-1, -1);
  #endif

  // calculate first column
  for(int i = 1; i < m; i++)
  {
    cost[i][0] = metric->reduce(metric->init(), cost[i-1][0], a[i], b[0]);
    ncost[i][0] = metric->norm(cost[i][0], a, b);
    #if 0
    trace[i][0] = std::make_pair(i - 1, 0);
    #endif
  }

  // calculate first row
  for(int j = 1; j < n; j++)
  {
    cost[0][j] = metric->reduce(metric->init(), cost[0][j-1], a[0], b[j]);
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
      data_t* minPrev = nullptr;
      if (ncost[i-1][j-1] < ncost[i][j-1] && ncost[i-1][j-1] < ncost[i-1][j])
      {
        minPrev = cost[i-1][j-1];
      }
      else if (ncost[i][j-1] < ncost[i-1][j])
      {
        minPrev = cost[i][j-1];
      }
      else {
        minPrev = cost[i-1][j];
      }
      cost[i][j] = metric->reduce(metric->init(), minPrev, a[i], b[j]);
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

  //deallocate2Darray(cost, m);
  for(int i = 1; i < m; i++)
  {
    for(int j = 1; j < n; j++)
    {
      delete cost[i][j];
    }
  }

  return result;
}



  #if 0
  //TODO: warping_path
  if (warping_path != NULL)
  {
    int i = m - 1, j = n - 1;
    while (i != -1)
    {
      warping_path->push_back(make_pair(i, j));
      int old_i = i, old_j = j;
      i = trace[old_i][old_j].first;
      j = trace[old_i][old_j].second;
    }
  }

  // during warping add correct values
  if (warping_path != NULL)
  {
    vector<data_t> tmp;
    tmp.push_back(cost[i - 1][j]);
    tmp.push_back(cost[i][j - 1]);
    tmp.push_back(cost[i - 1][j - 1]);

    auto mpe = min_element(tmp.begin(), tmp.end());
    if (mpe == tmp.begin())
    {
      trace[i][j] = make_pair(i - 1, j);
    }
    else if (mpe == tmp.begin() + 1)
    {
      trace[i][j] = make_pair(i, j - 1);
    }
    else if (mpe == tmp.begin() + 2)
    {
      trace[i][j] = make_pair(i - 1, j - 1);
    }
  }

  // clean up
  for (int i = 0; i < m; i++)
  {
    delete trace[i];
  }

  delete[] trace;
  #endif


/**
 *  @brief returns the distance between two sets of data
 *
 *  @param metric the distance metric to use
 *  @param x_1 one of the two arrays of data
 *  @param x_2 the other of the two arrays of data
 */
data_t generalDistance(const DistanceMetric* metric,
                       const TimeSeries& x_1,
                       const TimeSeries& x_2,
                       data_t dropout)
{
  if (x_1.getLength() != x_2.getLength())
  {
    throw GenexException("Two time series must have the same length for pairwise distance");
  }

  data_t* total = metric->init();
  bool dropped = false;

  for(int i = 0; i < x_1.getLength(); i++)
  {
    metric->reduce(total, total, x_1[i], x_2[i]);
    if (metric->norm(total, x_1, x_2) >= dropout)
    {
      dropped = true;
      break;
    }
  }

  data_t result = dropped ? INF : metric->norm(total, x_1, x_2);
  delete total;

  return result;
}

} // namespace distance
} // namespace genex

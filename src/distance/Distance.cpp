#include "distance/Distance.hpp"
#include "Exception.hpp"

#include <map>

#include "Exception.hpp"
#include "distance/DistanceMetric.hpp"
#include "distance/Euclidean.hpp"
#include "distance/Manhattan.hpp"
#include "distance/Chebyshev.hpp"

#include <iostream>
bool gDebug;
namespace genex {
namespace distance {

const Cache* minCache(const Cache* c1, const Cache* c2)
{
  return c1->lessThan(c2) ? c1 : c2;
}

static std::vector<const DistanceMetric*> gAllMetric =
  {
    new Euclidean(),
    new Manhattan(),
    new Chebyshev()
  };

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

template<typename T>
T** allocate2DArray(int nrow, int ncol)
{
  T** arr = new T*[nrow];
  for (int i = 0; i < nrow; i++)
  {
    arr[i] = new T[ncol];
  }
  return arr;
}

template<typename T>
void deallocate2Darray(T** arr, int nrow)
{
  for (int i = 0; i < nrow; i++)
  {
    delete[] arr[i];
  }
  delete[] arr;
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
    return metric->dist(a[0], b[0]);
  }

  // create cost matrix
  //Cache** cost = allocate2DArray<data_t>(m, n);
  std::vector< std::vector< Cache* > > cost(m, std::vector<Cache*>(n));

  #if 0
  auto trace = new std::pair<data_t, data_t>*[m]; // For tracing warping
  for (int i = 0; i < m; i++)
  {
      trace[i] = new std::pair<data_t, data_t>[n];
  }
  #endif

  Cache* init = metric->init();
  cost[0][0] = metric->reduce(init, a[0], b[0]);
  delete init;

  #if 0
  trace[0][0] = std::make_pair(-1, -1);
  #endif

  // calculate first column
  for(int i = 1; i < m; i++)
  {
      cost[i][0] = metric->reduce(cost[i-1][0], a[i], b[0]);
      #if 0
      trace[i][0] = std::make_pair(i - 1, 0);
      #endif
  }

  // calculate first row
  for(int j = 1; j < n; j++)
  {
    cost[0][j] = metric->reduce(cost[0][j-1], a[0], b[j]);
    #if 0
    trace[0][j] = std::make_pair(0, j - 1);
    #endif
  }

  data_t result;
  bool dropped = false;

  // fill matrix. If using dropout, keep tabs on min cost per row.
  for(int i = 1; i < m; i++)
  {
    const Cache* bestSoFar = cost[i][0];
    for(int j = 1; j < n; j++)
    {
      //TODO switch to norm, for more complicated dists < may not work
      //error is likely here
      const Cache* g1 = minCache(cost[i-1][j], cost[i][j-1]);
      const Cache* mp = minCache(g1, cost[i-1][j-1]);
      //Cache* mp = std::min({cost[i-1][j], cost[i][j-1], cost[i-1][j-1]});
      cost[i][j] = metric->reduce(mp, a[i], b[j]);
      bestSoFar = minCache(bestSoFar, cost[i][j]);
    }

    if (metric->norm(bestSoFar, a, b) > dropout)  //Short circuit calculation.
    {
      dropped = true;
    }
  }

  if (dropped)
  {
    result = INF;
  }
  else
  {
    result = metric->norm(cost[m - 1][n - 1], a, b);
  }
  // if (gDebug)
  // {
  //   for (int i = m - 1; i >= 0; i--)
  //   {
  //     for (int j = 0; j < n; j++)
  //     {
  //       std::cout << pow(cost[i][j]->val, 2) << " ";
  //     }
  //     std::cout << std::endl;
  //   }
  // }
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
                       const TimeSeries& x_2)
{
  if (x_1.getLength() != x_2.getLength())
  {
    throw GenexException("Two time series must have the same length for general distance (pairwise)");
  }

  Cache* total = metric->init();
  Cache* curr;

  for(int i = 0; i < x_1.getLength(); i++)
  {
    curr = metric->reduce(total, x_1[i], x_2[i]);
    delete total;
    total = curr;
  }

  data_t result = metric->norm(total, x_1, x_2);
  delete total;

  return result;
}


} // namespace distance
} // namespace genex

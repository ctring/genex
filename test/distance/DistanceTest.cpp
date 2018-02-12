#define BOOST_TEST_MODULE "Test General Distance Function"

#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>

#include "distance/Euclidean.hpp"
#include "distance/Chebyshev.hpp"
#include "distance/Manhattan.hpp"
#include "distance/Cosine.hpp"
#include "distance/Distance.hpp"
#include "Exception.hpp"
#include "TimeSeries.hpp"

using namespace genex;

#define TOLERANCE 1e-9
struct MockData
{
  dist_t euclidean_dist = pairwiseDistance<Euclidean, double>;
  dist_t manhattan_dist = pairwiseDistance<Manhattan, double>;
  dist_t chebyshev_dist = pairwiseDistance<Chebyshev, double>;

  dist_t euclidean_warped_dist = warpedDistance<Euclidean, double>;
  dist_t manhattan_warped_dist = warpedDistance<Manhattan, double>;
  dist_t chebyshev_warped_dist = warpedDistance<Chebyshev, double>;

  data_t dat_1[5] = {1, 2, 3, 4, 5};
  data_t dat_2[5] = {11, 2, 3, 4, 5};

  data_t dat_3[2] = {2, 4};
  data_t dat_4[5] = {2, 2, 2, 4, 4};

  data_t dat_5[4] = {1, 2, 2, 4};
  data_t dat_6[4] = {1, 2, 4, 5};

  data_t dat_7[4] = {2, 2, 2, 2};
  data_t dat_8[4] = {20, 20, 20, 15};

  data_t dat_9[6]  = {2, 2, 2, 2, 2, 2};
  data_t dat_10[6] = {4, 3, 3, 3, 3, 3};

  data_t dat_11[7] = {4, 3, 5, 3, 5, 3, 4};
  data_t dat_12[7] = {4, 3, 3, 1, 1, 3, 4};

  data_t dat_13[10] = {0, 2, 3, 5, 8, 6, 3, 2, 3, 5};
  data_t dat_14[7] =  {8, 4, 6, 1, 5, 10, 9};
};

BOOST_AUTO_TEST_CASE( general_distance, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);

  data_t total_1 = data.euclidean_dist(ts_1, ts_2, INF);
  BOOST_TEST( total_1, 2.0 );

  data_t total_2 = data.manhattan_dist(ts_1, ts_2, INF);
  BOOST_TEST( total_2, 2.0 );

  data_t total_3 = data.chebyshev_dist(ts_1, ts_2, INF);
  BOOST_TEST( total_3, 10.0 );
}

BOOST_AUTO_TEST_CASE( easy_general_warped_distance, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_1{data.dat_1, 0, 0, 2};
  TimeSeries ts_2{data.dat_2, 0, 0, 2};

  TimeSeries ts_3{data.dat_3, 0, 0, 2};
  TimeSeries ts_4{data.dat_4, 0, 0, 5};

  TimeSeries ts_5{data.dat_5, 0, 0, 4};
  TimeSeries ts_6{data.dat_6, 0, 0, 4};

  TimeSeries ts_11{data.dat_11, 0, 0, 7};
  TimeSeries ts_12{data.dat_12, 0, 0, 7};

  setWarpingBandRatio(1.0);

  data_t total_0 = data.euclidean_warped_dist(ts_1, ts_2, INF);
  BOOST_TEST( total_0 == sqrt(100.0) / (2 * 2) );

  data_t total_1 = data.euclidean_warped_dist(ts_3, ts_4, INF);
  BOOST_TEST( total_1 == 0.0 );

  data_t total_2 = data.manhattan_warped_dist(ts_3, ts_4, INF);
  BOOST_TEST( total_2 == 0.0 );

  data_t total_3 = data.chebyshev_warped_dist(ts_3, ts_4, INF);
  BOOST_TEST( total_3 == 0.0 );

  data_t total_4 = data.euclidean_warped_dist(ts_5, ts_6, INF);
  BOOST_TEST( total_4 == sqrt(1.0) / (2 * 4.0) );

  data_t total_5 = data.manhattan_warped_dist(ts_5, ts_6, INF);
  BOOST_TEST( total_5 == 1.0/ (2 * 4.0) );

  data_t total_6 = data.chebyshev_warped_dist(ts_5, ts_6, INF);
  BOOST_TEST( total_6 == 1.0 );

  data_t total_7 = data.euclidean_warped_dist(ts_11, ts_12, INF);
  data_t result_7 = sqrt(12.0)/ (2 * 7);
  BOOST_TEST( total_7 == result_7 );

  data_t total_8 = data.manhattan_warped_dist(ts_11, ts_12, INF);
  BOOST_TEST( total_8 == 8.0 / (2 * 7) );

  data_t total_9 = data.chebyshev_warped_dist(ts_11, ts_12, INF);
  BOOST_TEST( total_9 == (2.0) );
}

BOOST_AUTO_TEST_CASE( easy_gwd_dropout, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_3{data.dat_3, 0, 0, 2};
  TimeSeries ts_4{data.dat_4, 0, 0, 5};

  TimeSeries ts_7{data.dat_7, 0, 0, 4};
  TimeSeries ts_8{data.dat_8, 0, 0, 4};

  data_t total_1 = data.euclidean_warped_dist(ts_3, ts_4, 5);
  BOOST_TEST( total_1 == 0.0 );

  data_t total_2 = data.manhattan_warped_dist(ts_3, ts_4, 5);
  BOOST_TEST( total_2 == 0.0 );

  data_t total_3 = data.chebyshev_warped_dist(ts_3, ts_4, 5);
  BOOST_TEST( total_3 == 0.0 );

  data_t total_5 = data.manhattan_warped_dist(ts_7, ts_8, 5);
  BOOST_TEST( isinf(total_5) );

  data_t total_6 = data.chebyshev_warped_dist(ts_7, ts_8, 5);
  BOOST_TEST( isinf(total_6) );
}

BOOST_AUTO_TEST_CASE( gwd_different_distances, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_9{data.dat_9, 0, 0, 6};
  TimeSeries ts_10{data.dat_10, 0, 0, 6};

  data_t total_1 = data.euclidean_warped_dist(ts_9, ts_10, INF);
  BOOST_TEST( total_1 == sqrt(9.0)/(2 * 6) );

  data_t total_2 = data.manhattan_warped_dist(ts_9, ts_10, INF);
  BOOST_TEST( total_2 == 7.0/ (2 * 6) );

  data_t total_3 = data.chebyshev_warped_dist(ts_9, ts_10, INF);
  BOOST_TEST( total_3 == 2.0 );
}

BOOST_AUTO_TEST_CASE( get_distance_metric, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_for_function_call{data.dat_3, 0, 0, 2};
  const dist_t d = getDistanceFromName("euclidean");
  BOOST_CHECK(d);
}

BOOST_AUTO_TEST_CASE( distance_not_found )
{
  BOOST_CHECK_THROW( getDistanceFromName("oracle"), GenexException );
}

BOOST_AUTO_TEST_CASE( keogh_lower_bound, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries a{data.dat_13, 10};
  TimeSeries b{data.dat_14, 7};

  setWarpingBandRatio(0.2);
  data_t klb = keoghLowerBound(a, b, 10);

  BOOST_TEST( klb == sqrt(31.0) / (2 * 10) );
}

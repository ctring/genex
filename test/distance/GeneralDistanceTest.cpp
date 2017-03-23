#define BOOST_TEST_MODULE "Test General Distance Function"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"
#include "distance/Chebyshev.hpp"
#include "distance/Manhattan.hpp"

#include "distance/Distance.hpp"

#include <iostream> //debug

using namespace genex;

#define TOLERANCE 1e-9
struct MockData
{
  data_t dat_1[5] = {1, 2, 3, 4, 5};
  data_t dat_2[5] = {11, 2, 3, 4, 5};

  data_t dat_3[5] = {2, 2, 2, 4, 4};
  data_t dat_4[2] = {2, 4};

  data_t dat_5[4] = {1, 2, 2, 4};
  data_t dat_6[4] = {1, 2, 4, 5};

  data_t dat_7[4] = {2, 2, 2, 2};
  data_t dat_8[4] = {9, 9, 8, 9};

  data_t dat_9[6]  = {2, 2, 2, 2, 2, 2};
  data_t dat_10[6] = {4, 3, 3, 3, 3, 3};
};

BOOST_AUTO_TEST_CASE( general_distance, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);
  Euclidean dist_1;
  Manhattan dist_2;
  Chebyshev dist_3;

  data_t total_1 = generalDistance(dist_1, ts_1, ts_2);
  BOOST_TEST( total_1, 2.0 );

  data_t total_2 = generalDistance(dist_2, ts_1, ts_2);
  BOOST_TEST( total_2, 2.0 );

  data_t total_3 = generalDistance(dist_3, ts_1, ts_2);
  BOOST_TEST( total_3, 10.0 );
}

BOOST_AUTO_TEST_CASE( easy_general_warped_distance, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_3{data.dat_3, 0, 0, 5};
  TimeSeries ts_4{data.dat_4, 0, 0, 2};

  TimeSeries ts_5{data.dat_5, 0, 0, 4};
  TimeSeries ts_6{data.dat_6, 0, 0, 4};

  Euclidean dist_1;
  Manhattan dist_2;
  Chebyshev dist_3;

  data_t total_1 = generalWarpedDistance(dist_1, ts_3, ts_4, INF);
  BOOST_TEST( total_1 == 0.0 );

  data_t total_2 = generalWarpedDistance(dist_2, ts_3, ts_4, INF);
  BOOST_TEST( total_2 == 0.0 );

  data_t total_3 = generalWarpedDistance(dist_3, ts_3, ts_4, INF);
  BOOST_TEST( total_3 == 0.0 );

  data_t total_4 = generalWarpedDistance(dist_1, ts_5, ts_6, INF);
  BOOST_TEST( total_4 == 1.0 );

  data_t total_5 = generalWarpedDistance(dist_2, ts_5, ts_6, INF);
  BOOST_TEST( total_5 == 1.0 );

  data_t total_6 = generalWarpedDistance(dist_3, ts_5, ts_6, INF);
  BOOST_TEST( total_6 == 1.0 );
}

BOOST_AUTO_TEST_CASE( easy_gwd_dropout, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_3{data.dat_3, 0, 0, 5};
  TimeSeries ts_4{data.dat_4, 0, 0, 2};

  TimeSeries ts_7{data.dat_7, 0, 0, 4};
  TimeSeries ts_8{data.dat_8, 0, 0, 4};

  Euclidean dist_1;
  Manhattan dist_2;
  Chebyshev dist_3;

  data_t total_1 = generalWarpedDistance(dist_1, ts_3, ts_4, 5);
  BOOST_TEST( total_1 == 0.0 );

  data_t total_2 = generalWarpedDistance(dist_2, ts_3, ts_4, 5);
  BOOST_TEST( total_2 == 0.0 );

  data_t total_3 = generalWarpedDistance(dist_3, ts_3, ts_4, 5);
  BOOST_TEST( total_3 == 0.0 );

  data_t total_4 = generalWarpedDistance(dist_1, ts_7, ts_8, 5);
  BOOST_TEST( total_4 == INF );

  data_t total_5 = generalWarpedDistance(dist_2, ts_7, ts_8, 5);
  BOOST_TEST( total_5 == INF );

  data_t total_6 = generalWarpedDistance(dist_3, ts_7, ts_8, 5);
  BOOST_TEST( total_6 == INF );
}

BOOST_AUTO_TEST_CASE( gwd_different_distances, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_9{data.dat_9, 0, 0, 6};
  TimeSeries ts_10{data.dat_10, 0, 0, 6};

  Euclidean dist_1;
  Manhattan dist_2;
  Chebyshev dist_3;

  data_t total_1 = generalWarpedDistance(dist_1, ts_9, ts_10, INF);
  BOOST_TEST( total_1 == 9.0 );

  data_t total_2 = generalWarpedDistance(dist_2, ts_9, ts_10, INF);
  BOOST_TEST( total_2 == 7.0 );

  data_t total_3 = generalWarpedDistance(dist_3, ts_9, ts_10, INF);
  BOOST_TEST( total_3 == 2.0 );
}

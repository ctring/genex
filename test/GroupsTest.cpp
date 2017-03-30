#define BOOST_TEST_MODULE "Test Groups class"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"
#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  data_t dat_1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  data_t dat_2[10] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5};

  data_t centroid_1[5] = {1, 1.5, 2.5, 3, 4};

  data_t dat_3[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  data_t dat_4[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  data_t dat_5[10] = {9, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  data_t dat_sim_1[5] = {1, 2, 3, 4, 5};
  data_t dat_sim_2[5] = {2, 3, 4, 5, 6};
  data_t dat_diff[5]  = {12, 13, 14, 15};

  std::string test_5_10_space = "dataset/test/test_5_10_space.txt";
  std::string test_3_10_space = "dataset/test/test_3_10_space.txt";
};

BOOST_AUTO_TEST_CASE( basic_groups, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;

  int timeSeriesCount = 5;
  int timeSeriesLengths = 10;
  int memberLength = 5;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_5_10_space, timeSeriesCount, 0, " ");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), timeSeriesLengths );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), timeSeriesCount );
  BOOST_CHECK( tsSet.getFilePath() == data.test_5_10_space );

  Group g(tsSet, memberLength);
  TimeSeries& c = g.getCentroid();

  //test initial centroid is all zeros
  for (int i = 0; i < memberLength; i++)
  {
    BOOST_TEST( c[i] == 0 );
  }

  g.addMember(0, 0);
  c = g.getCentroid();

  //checking if centroid is updated
  for (int i = 0; i < memberLength; i++)
  {
    BOOST_TEST( c[i] == data.dat_1[i] );
  }

  BOOST_CHECK_EQUAL( g.getCount(), 1 );

  g.addMember(1, 0);
  c = g.getCentroid();

  BOOST_CHECK_EQUAL( g.getCount(), 2 );

  //checking if centroid is updated
  for (int i = 0; i < memberLength; i++)
  {
    BOOST_TEST( c[i] == data.centroid_1[i] );
  }
}

BOOST_AUTO_TEST_CASE( group_get_best_match, *boost::unit_test::tolerance(TOLERANCE) )
{
  //TODO(complete test)
  MockData data;
  Euclidean metric;

  int timeSeriesCount = 3;
  int timeSeriesLengths = 10;
  int memberLength = 10;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, timeSeriesCount, 0, " ");

  Group g(tsSet, memberLength);
  g.addMember(2, 0);
  g.addMember(0, 0);
  TimeSeries t = tsSet.getTimeSeries(1,0,memberLength);
  BOOST_TEST(t[0] == 1.0);
  candidate_t best = g.getBestMatch(t, &metric);
  BOOST_TEST(best.dist == 1.0/10.0);
}

BOOST_AUTO_TEST_CASE( group_id )
{
  MockData data;

  TimeSeriesSet tsSet;
  int timeSeriesCount = 5;
  tsSet.loadData(data.test_5_10_space, timeSeriesCount, 0, " ");

  Group g_1(tsSet, 1);
  Group g_2(tsSet, 1);
  BOOST_CHECK_PREDICATE( std::not_equal_to<int>(),  (g_1.getId()) (g_2.getId()) );
}

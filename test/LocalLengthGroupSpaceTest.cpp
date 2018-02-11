#define BOOST_TEST_MODULE "Test LocalLengthGroupSpace class"

#include <boost/test/unit_test.hpp>
#include <cstdio>
#include "Common.hpp"
#include "LocalLengthGroupSpace.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Euclidean.hpp"
#include "distance/Distance.hpp"
#include "Exception.hpp"
#include "Group.hpp"

using namespace genex;

struct MockData
{
  std::string test_group_5_10_space = "datasets/test/test_group_5_10_space.txt";
  std::string test_group_5_10_different_space = "datasets/test/test_group_5_10_different_space.txt";
};

BOOST_AUTO_TEST_CASE( local_length_group_space, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  dist_t distance = pairwiseDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_space, 5, 0, " ");

  LocalLengthGroupSpace groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( distance, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 1 );
}

BOOST_AUTO_TEST_CASE( local_length_group_space_different, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  dist_t distance = pairwiseDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  LocalLengthGroupSpace groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( distance, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 2);
}

BOOST_AUTO_TEST_CASE( groups_best_group, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  dist_t distance = warpedDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");
  
  LocalLengthGroupSpace groups(tsSet, 10);
  groups.generateGroups( distance, 0.5 );

  setWarpingBandRatio(1.0);

  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,9), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,8), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,6), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,5), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,4), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,4,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,5,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(0,3,7), distance, INF).first);

  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,9), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,8), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,6), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,5), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,4), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,4,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,5,10), distance, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,6,10), distance, INF).first);
}

BOOST_AUTO_TEST_CASE( groups_save_and_load, *boost::unit_test::tolerance(EPS) )
{
  // Set up
  MockData data;
  std::string fname = "groups_save_and_load.z";
  dist_t distance = warpedDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");
  
  LocalLengthGroupSpace groups(tsSet, 10);
  groups.generateGroups( distance, 0.5 );

  // Save
  saveToFile(groups, fname);

  // Load
  LocalLengthGroupSpace groups2(tsSet, 10);
  loadFromFile(groups2, fname);

  // Compare
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), groups2.getNumberOfGroups() );

  remove(fname.c_str());
}
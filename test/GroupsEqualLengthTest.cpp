#define BOOST_TEST_MODULE "Test GroupsEqualLength class"

#include <boost/test/unit_test.hpp>
#include "LocalLengthGroupSpace.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Euclidean.hpp"
#include "distance/Distance.hpp"
#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  std::string test_group_5_10_space = "datasets/test/test_group_5_10_space.txt";
  std::string test_group_5_10_different_space = "datasets/test/test_group_5_10_different_space.txt";
};

BOOST_AUTO_TEST_CASE( groups_equal_length, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  dist_t distance = pairwiseDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_space, 5, 0, " ");

  GroupsEqualLength groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( distance, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 1 );
}

BOOST_AUTO_TEST_CASE( groups_equal_length_different, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  dist_t distance = pairwiseDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  GroupsEqualLength groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( distance, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 2);
}

BOOST_AUTO_TEST_CASE( groups_best_group, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  dist_t distance = warpedDistance<Euclidean, data_t>;
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");
  
  GroupsEqualLength groups(tsSet, 10);
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

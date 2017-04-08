#define BOOST_TEST_MODULE "Test GroupsEqualLength class"

#include <boost/test/unit_test.hpp>
#include "GroupsEqualLength.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Euclidean.hpp"
#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  std::string test_group_5_10_space = "dataset/test/test_group_5_10_space.txt";
  std::string test_group_5_10_different_space = "dataset/test/test_group_5_10_different_space.txt";
};

BOOST_AUTO_TEST_CASE( groups_equal_length, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  DistanceMetric* metric = new Euclidean();
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_space, 5, 0, " ");

  GroupsEqualLength groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( metric, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 1 );
}

BOOST_AUTO_TEST_CASE( groups_equal_length_different, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  DistanceMetric* metric = new Euclidean();
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  GroupsEqualLength groups(tsSet, 10);

  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 0 );

  groups.generateGroups( metric, 0.5 );
  BOOST_CHECK_EQUAL( groups.getNumberOfGroups(), 2);
}

BOOST_AUTO_TEST_CASE( groups_best_group, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  DistanceMetric* metric = new Euclidean();
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  GroupsEqualLength groups(tsSet, 10);
  groups.generateGroups( metric, 0.5 );
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,9), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,8), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,6), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,5), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,0,4), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,4,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(1,5,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(0), groups.getBestGroup(tsSet.getTimeSeries(0,3,7), metric, INF).first);

  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,9), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,8), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,6), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,5), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,0,4), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,4,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,5,10), metric, INF).first);
  BOOST_CHECK_EQUAL( groups.getGroup(1), groups.getBestGroup(tsSet.getTimeSeries(4,6,10), metric, INF).first);
}

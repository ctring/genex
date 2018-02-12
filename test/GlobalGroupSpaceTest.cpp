#define BOOST_TEST_MODULE "Test LocalLengthGroupSpace class"

#include <boost/test/unit_test.hpp>
#include <cstdio>
#include "IO.hpp"
#include "GlobalGroupSpace.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Distance.hpp"
#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  data_t dat[7] = {110, 116, 118, 117, 16.5, 112, 112};
  std::string test_group_5_10_space = "datasets/test/test_group_5_10_space.txt";
  std::string test_group_5_10_different_space = "datasets/test/test_group_5_10_different_space.txt";
  std::string ItalyPowerDemand = "datasets/test/ItalyPowerDemand_DATA";
};

BOOST_AUTO_TEST_CASE( local_length_group_space, *boost::unit_test::tolerance(EPS) )
{
  MockData data;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  BOOST_CHECK_EQUAL(tsSet.getItemCount(), 5);
  BOOST_CHECK_EQUAL(tsSet.getItemLength(), 10);

  GlobalGroupSpace gSet(tsSet);
  gSet.group("euclidean", 0.5);
  auto best = gSet.getBestMatch(tsSet.getTimeSeries(0, 0, 10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,4,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,6,9));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,2,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,3,7));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,0,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,4,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,6,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,2,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,3,7));
  BOOST_TEST((best.dist) == 0);
  TimeSeries ts1(data.dat, 0,0,7);
  best = gSet.getBestMatch(ts1);
  BOOST_TEST((best.dist)> 0);
}

BOOST_AUTO_TEST_CASE( local_length_group_space_multi_threaded, *boost::unit_test::tolerance(EPS) )
{
  MockData data;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  BOOST_CHECK_EQUAL(tsSet.getItemCount(), 5);
  BOOST_CHECK_EQUAL(tsSet.getItemLength(), 10);

  GlobalGroupSpace gSet(tsSet);
  gSet.groupMultiThreaded("euclidean", 0.5, 4);
  auto best = gSet.getBestMatch(tsSet.getTimeSeries(0, 0, 10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,4,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,6,9));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,2,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,3,7));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,0,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,4,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,6,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,2,10));
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(4,3,7));
  BOOST_TEST((best.dist) == 0);
  TimeSeries ts1(data.dat, 0,0,7);
  best = gSet.getBestMatch(ts1);
  BOOST_TEST((best.dist)> 0);
}

BOOST_AUTO_TEST_CASE( traverse_order )
{
  setWarpingBandRatio(0.4);
  vector<int> order = generateTraverseOrder(3, 7);
  vector<int> expected = { 3, 2, 4, 5 };
  BOOST_CHECK_EQUAL_COLLECTIONS(order.begin(), order.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( global_group_space_save_load )
{
  // Set up
  MockData data;
  std::string fname = "global_group_space_save_load.z";
  TimeSeriesSet tsSet;
  tsSet.loadData(data.ItalyPowerDemand, 5, 0, " ");

  GlobalGroupSpace ggs(tsSet);
  ggs.group("euclidean", 0.5);

  // Save
  saveToFile(ggs, fname);

  // Load
  GlobalGroupSpace ggs2(tsSet);
  loadFromFile(ggs2, fname);

  // Compare
  auto best = ggs.getBestMatch(tsSet.getTimeSeries(0,4,10));
  auto best2 = ggs2.getBestMatch(tsSet.getTimeSeries(0,4,10));
  bool ok = best == best2;
  BOOST_TEST( ok );
  BOOST_CHECK_EQUAL( ggs.getDistanceName(), ggs2.getDistanceName() );
  BOOST_CHECK_EQUAL( ggs.getThreshold(), ggs2.getThreshold() );
  BOOST_CHECK_EQUAL( ggs.getTotalNumberOfGroups(), ggs2.getTotalNumberOfGroups() );
  remove(fname.c_str());
}
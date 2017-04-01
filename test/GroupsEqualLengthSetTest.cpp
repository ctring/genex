#define BOOST_TEST_MODULE "Test GroupsEqualLength class"

#include <boost/test/unit_test.hpp>
#include "GroupsEqualLengthSet.hpp"
#include "TimeSeriesSet.hpp"
#include "distance/Euclidean.hpp"
#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  data_t dat[7] = {110, 116, 118, 117, 16.5, 112, 112};
  std::string test_group_5_10_space = "dataset/test/test_group_5_10_space.txt";
  std::string test_group_5_10_different_space = "dataset/test/test_group_5_10_different_space.txt";
};

BOOST_AUTO_TEST_CASE( groups_equal_length, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  DistanceMetric* metric = new Euclidean();
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_group_5_10_different_space, 5, 0, " ");

  BOOST_CHECK_EQUAL(tsSet.getItemCount(), 5);
  BOOST_CHECK_EQUAL(tsSet.getItemLength(), 10);
  data_t out_dist;

  GroupsEqualLengthSet gSet(tsSet);
  gSet.group(metric, 0.5);


  candidate_t best = gSet.getBestMatch(tsSet.getTimeSeries(0,0,10), metric);
  BOOST_TEST((best.dist) == 0);
  best = gSet.getBestMatch(tsSet.getTimeSeries(0,4,10), metric);
  // BOOST_TEST((best.dist) == 0);
  // // best = gSet.getBestMatch(tsSet.getTimeSeries(0,6,9), metric); //fails
  // // BOOST_TEST((best.dist) == 0);
  // best = gSet.getBestMatch(tsSet.getTimeSeries(0,2,10), metric);
  // BOOST_TEST((best.dist) == 0);
  // // best = gSet.getBestMatch(tsSet.getTimeSeries(0,3,7), metric); //fails
  // // BOOST_TEST((best.dist) == 0);
  // best = gSet.getBestMatch(tsSet.getTimeSeries(4,0,10), metric);
  // BOOST_TEST((best.dist) == 0);
  // best = gSet.getBestMatch(tsSet.getTimeSeries(4,4,10), metric);
  // BOOST_TEST((best.dist) == 0);
  // // best = gSet.getBestMatch(tsSet.getTimeSeries(4,6,10), metric); //fails
  // // BOOST_TEST((best.dist) == 0);
  // best = gSet.getBestMatch(tsSet.getTimeSeries(4,2,10), metric);
  // BOOST_TEST((best.dist) == 0);
  // // best = gSet.getBestMatch(tsSet.getTimeSeries(4,3,7), metric); //fails
  // // BOOST_TEST((best.dist) == 0);
  // TimeSeries ts1(data.dat, 0,0,7);
  // best = gSet.getBestMatch(ts1, metric);
  // BOOST_TEST((best.dist)> 0);
}

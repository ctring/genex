#define BOOST_TEST_MODULE "Test GroupableTimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "GroupableTimeSeriesSet.hpp"
#include "Exception.hpp"
#include "distance/Euclidean.hpp"
#include "Group.hpp"

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "dataset/test/test_10_20_space.txt";
  std::string test_3_10_space = "dataset/test/test_3_10_space.txt";
} data;

BOOST_AUTO_TEST_CASE( groupable_time_series_set_load_space )
{
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, 0, " ");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 10 );
  BOOST_CHECK( tsSet.getFilePath() == data.test_10_20_space );
}

BOOST_AUTO_TEST_CASE( groupable_time_series_grouping_and_reset )
{
  Euclidean* metric = new Euclidean();
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 20, 0, " ");
  int groupCnt = tsSet.groupAllLengths(metric, 0.5);
  BOOST_CHECK( groupCnt > 2 );
  groupCnt = tsSet.groupAllLengths(metric, 0.5);
  BOOST_CHECK( groupCnt > 2 );
}

BOOST_AUTO_TEST_CASE( groupable_time_series_not_grouped_exception )
{
  Euclidean* metric = new Euclidean();
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 20, 0, " ");
  
  // dataset not grouped:
  BOOST_CHECK_THROW( tsSet.getBestMatch(tsSet.getTimeSeries(0) , metric), GenexException );  
}

BOOST_AUTO_TEST_CASE( basic_get_best_match )
{
  Euclidean* metric = new Euclidean();
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 20, 0, " ");
  tsSet.groupAllLengths(metric, 0.5);
  candidate_time_series_t best = tsSet.getBestMatch(tsSet.getTimeSeries(0), metric);
  BOOST_TEST( best.dist == 0.0 );
}
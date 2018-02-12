#define BOOST_TEST_MODULE "Test GroupableTimeSeriesSet class"

#include <cstdio>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "Common.hpp"
#include "GroupableTimeSeriesSet.hpp"
#include "Exception.hpp"
#include "distance/Distance.hpp"
#include "Group.hpp"

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "datasets/test/test_10_20_space.txt";
  std::string test_3_10_space = "datasets/test/test_3_10_space.txt";
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
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 0, 0, " ");
  int groupCnt = tsSet.groupAllLengths("euclidean", 0.5, 1);
  BOOST_CHECK( groupCnt > 2 );
}

BOOST_AUTO_TEST_CASE( groupable_time_series_not_grouped_exception )
{
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 0, 0, " ");

  // dataset not grouped:
  BOOST_CHECK_THROW( tsSet.getBestMatch(tsSet.getTimeSeries(0)), GenexException );
}

BOOST_AUTO_TEST_CASE( basic_get_best_match )
{
  GroupableTimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 0, 0, " ");
  tsSet.normalize();
  tsSet.groupAllLengths("euclidean", 0.5, 1);
  candidate_time_series_t best = tsSet.getBestMatch(tsSet.getTimeSeries(0));
  BOOST_TEST( best.dist == 0.0 );
}

BOOST_AUTO_TEST_CASE( groupable_time_series_save_load )
{
  GroupableTimeSeriesSet tsSet;
  std::string fname = "groupable_time_series_save_load.z";
  tsSet.loadData(data.test_3_10_space, 0, 0, " ");
  tsSet.normalize();
  tsSet.groupAllLengths("euclidean", 0.1, 1);
  candidate_time_series_t best = tsSet.getBestMatch(tsSet.getTimeSeries(0));
  
  saveToFile(tsSet, fname);

  GroupableTimeSeriesSet tsSet2;
  tsSet2.loadData(data.test_3_10_space, 0, 0, " ");
  loadFromFile(tsSet2, fname);
  tsSet2.normalize();
  candidate_time_series_t best2 = tsSet2.getBestMatch(tsSet.getTimeSeries(0));

  bool ok = best == best2;
  BOOST_TEST( ok );
  BOOST_CHECK_EQUAL( tsSet.getDistanceName(), tsSet2.getDistanceName() );
  remove(fname.c_str());  
}

BOOST_AUTO_TEST_CASE( groupable_time_series_save_load_incompatible )
{
 GroupableTimeSeriesSet tsSet;
  std::string fname = "groupable_time_series_save_load_incompatible.z";
  tsSet.loadData(data.test_3_10_space, 0, 0, " ");
  tsSet.normalize();
  tsSet.groupAllLengths("euclidean", 0.1, 1);
  
  saveToFile(tsSet, fname);

  GroupableTimeSeriesSet tsSet2;
  tsSet2.loadData(data.test_10_20_space, 0, 0, " ");
  BOOST_CHECK_THROW( loadFromFile(tsSet2, fname), GenexException );
  remove(fname.c_str());  
}
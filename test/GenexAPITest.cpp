#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <iostream>     // std::cout
#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector

#include "TimeSeries.hpp"
#include "GenexAPI.hpp"
#include "Exception.hpp"

#include "group/Group.hpp"

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "datasets/test/test_10_20_space.txt";
  std::string test_15_20_comma = "datasets/test/test_15_20_comma.csv";
  std::string not_exist = "unicorn_santa_magic_halting_problem_solution";
  std::string uneven_rows = "datasets/test/uneven_rows.txt";
  std::string italy_power = "datasets/test/ItalyPowerDemand_DATA";
} data;

const bool timeSeriesEqual(const TimeSeries& a, const TimeSeries& b)
{
  return a.getLength() == b.getLength() 
      && a.getStart()  == b.getStart()
      && a.getIndex()  == b.getIndex();
}

const bool containsTimeSeries(const std::vector<TimeSeries> a, const TimeSeries& b)
{
  return std::any_of(a.begin(), a.end(), [&b](TimeSeries i){return timeSeriesEqual(i,b);});
}

const bool containsTimeSeries(const std::vector<candidate_time_series_t> a, const TimeSeries& b)
{
  return std::any_of(a.begin(), a.end(), [&b](candidate_time_series_t i){return timeSeriesEqual(i.data,b);});
}

BOOST_AUTO_TEST_CASE( api_load_dataset )
{
  GenexAPI api;
  string ds0 = api.loadDataset("test0", data.test_10_20_space, " ", 0, 0).name;
  string ds1 = api.loadDataset("test1", data.test_15_20_comma, ",", 10, 0).name;
  BOOST_CHECK_EQUAL( ds0, "test0" );
  BOOST_CHECK_EQUAL( ds1, "test1" );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 2 );
  BOOST_CHECK_THROW( api.loadDataset("test0", data.test_10_20_space, " ", 0, 0), GenexException);
  BOOST_CHECK_THROW( api.loadDataset("not_exist", data.not_exist, " ", 10, 0), GenexException );
  BOOST_CHECK_THROW( api.loadDataset("uneven", data.uneven_rows, " ", 10, 0), GenexException );
}

BOOST_AUTO_TEST_CASE( api_unload_dataset )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_15_20_comma, ",", 10, 0);
  api.loadDataset("test2", data.test_10_20_space, " ", 6, 0);
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadDataset("test1");

  BOOST_CHECK_THROW( api.unloadDataset("test1"), GenexException );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 2 );
}

BOOST_AUTO_TEST_CASE( api_unload_all_dataset )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_15_20_comma, ",", 10, 0);
  api.loadDataset("test2", data.test_10_20_space, " ", 6, 0);
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadAllDataset();
  BOOST_CHECK_THROW( api.unloadDataset("test0"), GenexException );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 0 );
}

BOOST_AUTO_TEST_CASE( api_get_dataset_info )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_15_20_comma, ",", 10, 4);

  dataset_metadata_t info = api.getDatasetInfo("test1");
  BOOST_CHECK_EQUAL( info.name, "test1" );
  BOOST_CHECK_EQUAL( info.itemCount, 10 );
  BOOST_CHECK_EQUAL( info.itemLength, 16 );

  api.loadDataset("test2", data.test_15_20_comma, ",", 10, 0);
  api.unloadDataset("test2");
  BOOST_CHECK_THROW( api.getDatasetInfo("test2"), GenexException );
}

BOOST_AUTO_TEST_CASE( api_group )
{
  GenexAPI api;
  api.loadDataset("test", data.test_10_20_space, " ", 5, 0);
 
  int count_1 = api.groupDataset("test", 0.5, "euclidean");
  int count_2 = api.groupDataset("test", 0.5, "chebyshev");
  BOOST_TEST( count_1 > 20 );
  BOOST_TEST( count_2 > 20 );
  BOOST_CHECK_THROW( api.groupDataset("test1", 0.5, "euclidean"), GenexException ); // no dataset test1 loaded
  BOOST_CHECK_THROW( api.groupDataset("test", 0.5, "oracle"), GenexException );     //no magical distance
}

BOOST_AUTO_TEST_CASE( api_match )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);

  int count_1 = api.groupDataset("test0", 0.5, "euclidean");

  candidate_time_series_t best_1 = api.getBestMatch("test0", "test0", 0);
  candidate_time_series_t best_2 = api.getBestMatch("test0", "test1", 0);
  candidate_time_series_t best_3 = api.getBestMatch("test0", "test1", 1);
  candidate_time_series_t best_4 = api.getBestMatch("test0", "test1", 0, 5, 10);

  BOOST_TEST(best_1.dist == 0.0);
  BOOST_TEST(best_2.dist == 0.0);
  BOOST_TEST(best_3.dist == 0.0);
  BOOST_TEST(best_4.dist == 0.0);

  BOOST_CHECK_THROW( api.getBestMatch("test1", "test0", 0), GenexException ); // dataset not grouped
  BOOST_CHECK_THROW( api.getBestMatch("test1", "test0", 35), GenexException ); // not that many ts in dataset
  BOOST_CHECK_THROW( api.getBestMatch("test1", "test0", 1, 100, 125), GenexException ); // not that big ts in dataset
}


BOOST_AUTO_TEST_CASE( api_knn_k_1 )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);

  int count_1 = api.groupDataset("test0", 0.5, "euclidean");
  api.setWarpingBandRatio(1.0);  
  
  std::vector<candidate_time_series_t> best_1 = api.getKBestMatches(1, 1, "test0", "test0", 0);
  std::vector<candidate_time_series_t> best_2 = api.getKBestMatches(1, 1, "test0", "test1", 0);
  std::vector<candidate_time_series_t> best_3 = api.getKBestMatches(1, 1, "test0", "test1", 1);
  std::vector<candidate_time_series_t> best_4 = api.getKBestMatches(1, 1, "test0", "test1", 0, 5, 10);
  
  candidate_time_series_t expected_1 = api.getBestMatch("test0", "test0", 0);
  candidate_time_series_t expected_2 = api.getBestMatch("test0", "test1", 0);
  candidate_time_series_t expected_3 = api.getBestMatch("test0", "test1", 1);
  candidate_time_series_t expected_4 = api.getBestMatch("test0", "test1", 0, 5, 10);
  BOOST_CHECK_EQUAL( best_1.size(), 1 );
  BOOST_CHECK_EQUAL( best_2.size(), 1 );
  BOOST_CHECK_EQUAL( best_3.size(), 1 );
  BOOST_CHECK_EQUAL( best_4.size(), 1 );
  
  BOOST_TEST(timeSeriesEqual(best_1[0].data, expected_1.data));
  BOOST_TEST(timeSeriesEqual(best_2[0].data, expected_2.data));
  BOOST_TEST(timeSeriesEqual(best_3[0].data, expected_3.data));
  BOOST_TEST(timeSeriesEqual(best_4[0].data, expected_4.data));
  
  BOOST_CHECK_THROW( api.getKBestMatches(1, 1, "test1", "test0", 0), GenexException ); // dataset not grouped
  BOOST_CHECK_THROW( api.getKBestMatches(1, 1, "test1", "test0", 35), GenexException ); // not that many ts in dataset
  BOOST_CHECK_THROW( api.getKBestMatches(1, 1, "test1", "test0", 1, 100, 125), GenexException ); // not that big ts in dataset
}

BOOST_AUTO_TEST_CASE( api_knn_k_2 )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);

  int count_1 = api.groupDataset("test0", 0.5, "euclidean");
  api.setWarpingBandRatio(1.0);  
  std::vector<candidate_time_series_t> best_1 = api.getKBestMatches(2, 2, "test0", "test0", 0);
  std::vector<candidate_time_series_t> best_2 = api.getKBestMatches(2, 2, "test0", "test1", 0);
  std::vector<candidate_time_series_t> best_3 = api.getKBestMatches(2, 2, "test0", "test1", 1);
  std::vector<candidate_time_series_t> best_4 = api.getKBestMatches(2, 2, "test0", "test1", 0, 5, 10);

  candidate_time_series_t expected_1 = api.getBestMatch("test0", "test0", 0);
  candidate_time_series_t expected_2 = api.getBestMatch("test0", "test1", 0);
  candidate_time_series_t expected_3 = api.getBestMatch("test0", "test1", 1);
  candidate_time_series_t expected_4 = api.getBestMatch("test0", "test1", 0, 5, 10);

  BOOST_CHECK_EQUAL( best_1.size(), 2 );
  BOOST_CHECK_EQUAL( best_2.size(), 2 );
  BOOST_CHECK_EQUAL( best_3.size(), 2 );
  BOOST_CHECK_EQUAL( best_4.size(), 2 );
  
  BOOST_TEST(containsTimeSeries(best_1, expected_1.data));
  BOOST_TEST(containsTimeSeries(best_2, expected_2.data));
  BOOST_TEST(containsTimeSeries(best_3, expected_3.data));
  BOOST_TEST(containsTimeSeries(best_4, expected_4.data));
}

BOOST_AUTO_TEST_CASE( api_knn_k_4 )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);
  
  int count_1 = api.groupDataset("test0", 0.5, "euclidean");
 
  api.setWarpingBandRatio(1.0); 
  std::vector<candidate_time_series_t> best_1 = api.getKBestMatches(4, 4, "test0", "test0", 0);
  std::vector<candidate_time_series_t> best_2 = api.getKBestMatches(4, 4, "test0", "test1", 0);
  std::vector<candidate_time_series_t> best_3 = api.getKBestMatches(4, 4, "test0", "test1", 1);
  std::vector<candidate_time_series_t> best_4 = api.getKBestMatches(4, 4, "test0", "test1", 0, 4, 10);

  candidate_time_series_t expected_1 = api.getBestMatch("test0", "test0", 0);
  candidate_time_series_t expected_2 = api.getBestMatch("test0", "test1", 0);
  candidate_time_series_t expected_3 = api.getBestMatch("test0", "test1", 1);
  candidate_time_series_t expected_4 = api.getBestMatch("test0", "test1", 0, 5, 10);

  BOOST_CHECK_EQUAL( best_1.size(), 4 );
  BOOST_CHECK_EQUAL( best_2.size(), 4 );
  BOOST_CHECK_EQUAL( best_3.size(), 4 );
  BOOST_CHECK_EQUAL( best_4.size(), 4 );
  
  BOOST_TEST(containsTimeSeries(best_1, expected_1.data));
  BOOST_TEST(containsTimeSeries(best_2, expected_2.data));
  BOOST_TEST(containsTimeSeries(best_3, expected_3.data));
  BOOST_TEST(containsTimeSeries(best_4, expected_4.data));
 }


BOOST_AUTO_TEST_CASE( api_kx_k_1 )
{
  GenexAPI api;
  api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
  api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);
  
  int count_1 = api.groupDataset("test0", 0.5, "euclidean");

  std::vector<candidate_time_series_t> best_1 = api.getKBestMatchesBruteForce(1, "test0", "test0", 0);
  std::vector<candidate_time_series_t> best_2 = api.getKBestMatchesBruteForce(1, "test0", "test1", 0);
  std::vector<candidate_time_series_t> best_3 = api.getKBestMatchesBruteForce(1, "test0", "test1", 1);
  std::vector<candidate_time_series_t> best_4 = api.getKBestMatchesBruteForce(1, "test0", "test1", 0, 5, 10);

  candidate_time_series_t expected_1 = api.getBestMatch("test0", "test0", 0);
  candidate_time_series_t expected_2 = api.getBestMatch("test0", "test1", 0);
  candidate_time_series_t expected_3 = api.getBestMatch("test0", "test1", 1);
  candidate_time_series_t expected_4 = api.getBestMatch("test0", "test1", 0, 5, 10);

  BOOST_CHECK_EQUAL( best_1.size(), 1 );
  BOOST_CHECK_EQUAL( best_2.size(), 1 );
  BOOST_CHECK_EQUAL( best_3.size(), 1 );
  BOOST_CHECK_EQUAL( best_4.size(), 1 );
  
  BOOST_TEST(timeSeriesEqual(best_1[0].data, expected_1.data));
  BOOST_TEST(timeSeriesEqual(best_2[0].data, expected_2.data));
  BOOST_TEST(timeSeriesEqual(best_3[0].data, expected_3.data));
  BOOST_TEST(timeSeriesEqual(best_4[0].data, expected_4.data));
 }

 BOOST_AUTO_TEST_CASE( api_kx_k_4 )
 {
   GenexAPI api;
   api.loadDataset("test0", data.test_10_20_space, " ", 5, 0);
   api.loadDataset("test1", data.test_10_20_space, " ", 5, 0);
   
   int count_1 = api.groupDataset("test0", 0.5, "euclidean");
 
   std::vector<candidate_time_series_t> best_1 = api.getKBestMatchesBruteForce(4, "test0", "test0", 0);
   std::vector<candidate_time_series_t> best_2 = api.getKBestMatchesBruteForce(4, "test0", "test1", 0);
   std::vector<candidate_time_series_t> best_3 = api.getKBestMatchesBruteForce(4, "test0", "test1", 1);
   std::vector<candidate_time_series_t> best_4 = api.getKBestMatchesBruteForce(4, "test0", "test1", 0, 5, 10);
 
   candidate_time_series_t expected_1 = api.getBestMatch("test0", "test0", 0);
   candidate_time_series_t expected_2 = api.getBestMatch("test0", "test1", 0);
   candidate_time_series_t expected_3 = api.getBestMatch("test0", "test1", 1);
   candidate_time_series_t expected_4 = api.getBestMatch("test0", "test1", 0, 5, 10);
 
   BOOST_CHECK_EQUAL( best_1.size(), 4 );
   BOOST_CHECK_EQUAL( best_2.size(), 4 );
   BOOST_CHECK_EQUAL( best_3.size(), 4 );
   BOOST_CHECK_EQUAL( best_4.size(), 4 );
   
   BOOST_TEST(containsTimeSeries(best_1, expected_1.data));
   BOOST_TEST(containsTimeSeries(best_2, expected_2.data));
   BOOST_TEST(containsTimeSeries(best_3, expected_3.data));
   BOOST_TEST(containsTimeSeries(best_4, expected_4.data));
}

BOOST_AUTO_TEST_CASE( save_load_groups, *boost::unit_test::tolerance(EPS)  )
{
  GenexAPI api;
  std::string dsName = "italy_power";
  std::string dsName2 = "italy_power2";
  std::string groupName = "italy_power_groups_test.z";
  api.loadDataset(dsName, data.italy_power, " ", 0, 0);
  api.groupDataset(dsName, 0.6);
  api.saveGroups(dsName, groupName);
  auto best = api.getBestMatch(dsName, dsName, 1, 4, 23);

  api.loadDataset(dsName2, data.italy_power, " ", 0, 0);
  api.loadGroups(dsName2, groupName);
  auto best2 = api.getBestMatch(dsName2, dsName2, 1, 4, 23);

  BOOST_CHECK_EQUAL(best.data, best2.data);
  BOOST_CHECK_EQUAL(best.dist, best2.dist);

  remove(groupName.c_str());  
}
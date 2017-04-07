#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "GenexAPI.hpp"
#include "Exception.hpp"

#include "Group.hpp"

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "dataset/test/test_10_20_space.txt";
  std::string test_15_20_comma = "dataset/test/test_15_20_comma.csv";
  std::string not_exist = "unicorn_santa_magic_halting_problem_solution";
  std::string uneven_rows = "dataset/test/uneven_rows.txt";
} data;

BOOST_AUTO_TEST_CASE( api_load_dataset )
{
  GenexAPI api;
  int id0 = api.loadDataset(data.test_10_20_space, 0, 0, " ").id;
  int id1 = api.loadDataset(data.test_15_20_comma, 10, 0, ",").id;
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id1, 1 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 2 );
  BOOST_CHECK_THROW( api.loadDataset(data.not_exist, 10, 0, " "), GenexException );
  BOOST_CHECK_THROW( api.loadDataset(data.uneven_rows, 10, 0, " "), GenexException );
}

BOOST_AUTO_TEST_CASE( api_unload_dataset )
{
  GenexAPI api;
  int id0 = api.loadDataset(data.test_10_20_space, 5, 0, " ").id;
  int id1 = api.loadDataset(data.test_15_20_comma, 10, 0, ",").id;
  int id2 = api.loadDataset(data.test_10_20_space, 6, 0, " ").id;
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id1, 1 );
  BOOST_CHECK_EQUAL( id2, 2 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadDataset(0);
  api.unloadDataset(2);

  BOOST_CHECK_THROW( api.unloadDataset(0), GenexException );
  BOOST_CHECK_THROW( api.unloadDataset(2), GenexException );

  BOOST_CHECK_EQUAL( api.getDatasetCount(), 1 );

  id0 = api.loadDataset(data.test_15_20_comma, 14, 4, ",").id;
  id2 = api.loadDataset(data.test_15_20_comma, 9, 0, ",").id;
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id2, 2 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );
}

BOOST_AUTO_TEST_CASE( api_unload_all_dataset )
{
  GenexAPI api;
  int id0 = api.loadDataset(data.test_10_20_space, 5, 0, " ").id;
  int id1 = api.loadDataset(data.test_15_20_comma, 10, 0, ",").id;
  int id2 = api.loadDataset(data.test_10_20_space, 6, 0, " ").id;
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadAllDataset();
  BOOST_CHECK_THROW( api.unloadDataset(0), GenexException );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 0 );
}

BOOST_AUTO_TEST_CASE( api_get_dataset_info )
{
  GenexAPI api;
  api.loadDataset(data.test_10_20_space, 5, 0, " ");
  api.loadDataset(data.test_15_20_comma, 10, 4, ",");

  dataset_info_t info = api.getDatasetInfo(1);
  BOOST_CHECK_EQUAL( info.name, data.test_15_20_comma );
  BOOST_CHECK_EQUAL( info.id, 1 );
  BOOST_CHECK_EQUAL( info.itemCount, 10 );
  BOOST_CHECK_EQUAL( info.itemLength, 16 );

  api.loadDataset(data.test_15_20_comma, 10, 0, ",");

  api.unloadDataset(1);
  BOOST_CHECK_THROW( api.getDatasetInfo(1), GenexException );
}

BOOST_AUTO_TEST_CASE( api_group )
{
  GenexAPI api;
  api.loadDataset(data.test_10_20_space, 5, 0, " ");
 
  int count_1 = api.groupDataset(0, 0.5, "euclidean");
  int count_2 = api.groupDataset(0, 0.5, "chebyshev");
  BOOST_TEST( count_1 > 20 );
  BOOST_TEST( count_2 > 20 );
  BOOST_CHECK_THROW( api.groupDataset(1, 0.5, "euclidean"), GenexException ); // no dataset 1 loaded
  BOOST_CHECK_THROW( api.groupDataset(0, 0.5, "oracle"), GenexException ); //no magical distance
}

BOOST_AUTO_TEST_CASE( api_match )
{
  GenexAPI api;
  api.loadDataset(data.test_10_20_space, 5, 0, " ");
  api.loadDataset(data.test_10_20_space, 5, 0, " ");

  int count_1 = api.groupDataset(0, 0.5, "euclidean");

  candidate_time_series_t best_1 = api.getBestMatch(0, 0, 0);
  candidate_time_series_t best_2 = api.getBestMatch(0, 1, 0);
  candidate_time_series_t best_3 = api.getBestMatch(0, 1, 1);
  candidate_time_series_t best_4 = api.getBestMatch(0, 1, 0, 5, 10);

  BOOST_TEST(best_1.dist == 0.0);
  BOOST_TEST(best_2.dist == 0.0);
  BOOST_TEST(best_3.dist == 0.0);
  BOOST_TEST(best_4.dist == 0.0);

  BOOST_CHECK_THROW( api.getBestMatch(1, 0, 0), GenexException ); // dataset not grouped
  BOOST_CHECK_THROW( api.getBestMatch(1, 0, 35), GenexException ); // not that many ts in dataset
  BOOST_CHECK_THROW( api.getBestMatch(1, 0, 1, 100, 125), GenexException ); // not that big ts in dataset
}


#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "GenexAPI.hpp"
#include "Exception.hpp"

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
  int id0 = api.loadDataset(data.test_10_20_space, 5);
  int id1 = api.loadDataset(data.test_15_20_comma, 10, ",");
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id1, 1 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 2 );
  BOOST_CHECK_THROW( api.loadDataset(data.not_exist, 10), GenexException );
  BOOST_CHECK_THROW( api.loadDataset(data.uneven_rows, 10), GenexException );
}

BOOST_AUTO_TEST_CASE( api_unload_dataset )
{
  GenexAPI api;
  int id0 = api.loadDataset(data.test_10_20_space, 5);
  int id1 = api.loadDataset(data.test_15_20_comma, 10, ",");
  int id2 = api.loadDataset(data.test_10_20_space, 6);
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id1, 1 );
  BOOST_CHECK_EQUAL( id2, 2 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadDataset(0);
  api.unloadDataset(2);

  BOOST_CHECK_THROW( api.unloadDataset(0), GenexException );
  BOOST_CHECK_THROW( api.unloadDataset(2), GenexException );

  BOOST_CHECK_EQUAL( api.getDatasetCount(), 1 );

  id0 = api.loadDataset(data.test_15_20_comma, 14, ",", 4);
  id2 = api.loadDataset(data.test_15_20_comma, 9, ",");
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id2, 2 );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );
}

BOOST_AUTO_TEST_CASE( api_unload_all_dataset )
{
  GenexAPI api;
  int id0 = api.loadDataset(data.test_10_20_space, 5);
  int id1 = api.loadDataset(data.test_15_20_comma, 10, ",");
  int id2 = api.loadDataset(data.test_10_20_space, 6);
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 3 );

  api.unloadAllDataset();
  BOOST_CHECK_THROW( api.unloadDataset(0), GenexException );
  BOOST_CHECK_EQUAL( api.getDatasetCount(), 0 );
}

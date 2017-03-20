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
  int id0 = loadDataset(data.test_10_20_space, 5);
  int id1 = loadDataset(data.test_15_20_comma, 10, ",");
  BOOST_CHECK_EQUAL( id0, 0 );
  BOOST_CHECK_EQUAL( id1, 1 );
  BOOST_CHECK_EQUAL( getDatasetCount(), 2 );
  BOOST_CHECK_THROW( loadDataset(data.not_exist, 10), GenexException );
  BOOST_CHECK_THROW( loadDataset(data.uneven_rows, 10), GenexException );
}
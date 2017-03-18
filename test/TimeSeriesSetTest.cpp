#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>
#include "TimeSeriesSet.hpp"
#include "Exception.hpp"
#include "TimeSeries.hpp"

#define TOLERANCE 1e-9

struct MockDataset
{
  std::string test_10_20_space = "dataset/test/test_10_20_space.txt";
  std::string test_15_20_comma = "dataset/test/test_15_20_comma.csv";
  std::string not_exist = "unicorn_santa_magic_halting_problem_solution";
  std::string uneven_rows = "dataset/test/uneven_rows.txt";
} data;

BOOST_AUTO_TEST_CASE( time_series_set_load_space, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet("test_10_20_space");
  tsSet.loadData(data.test_10_20_space, 20);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 10 );
  BOOST_CHECK( tsSet.getName() == std::string("test_10_20_space") );

  TimeSeries ts = tsSet.getTimeSeries(0);
  BOOST_TEST( ts[0] == 3.029296875 );
  BOOST_TEST( ts[ts.getLength() - 1] == 2.537109375);

  ts = tsSet.getTimeSeries(9);
  BOOST_TEST( ts[0] == 1.089843750 );
  BOOST_TEST( ts[ts.getLength() - 1] == 0.002822876);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_comma, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet("test_15_20_comma");
  tsSet.loadData(data.test_15_20_comma, 20, ",", 0);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 15 );
  BOOST_CHECK( tsSet.getName() == std::string("test_15_20_comma") );
}

BOOST_AUTO_TEST_CASE( time_series_set_load_file_not_exist )
{
  TimeSeriesSet tsSet("not_exist");
  BOOST_CHECK_THROW(tsSet.loadData(data.not_exist, 0), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_uneven_rows )
{
  TimeSeriesSet tsSet("uneven_rows");
  BOOST_CHECK_THROW(tsSet.loadData(data.uneven_rows, 10), GenexException);
}

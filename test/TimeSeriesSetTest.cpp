#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "TimeSeriesSet.hpp"
#include "Exception.hpp"
#include "TimeSeries.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "dataset/test/test_10_20_space.txt";
  std::string test_15_20_comma = "dataset/test/test_15_20_comma.csv";
  std::string not_exist = "unicorn_santa_magic_halting_problem_solution";
  std::string uneven_rows = "dataset/test/uneven_rows.txt";
} data;

BOOST_AUTO_TEST_CASE( time_series_set_load_space, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, " ", 0);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 10 );
  BOOST_CHECK( tsSet.getFilePath() == data.test_10_20_space );

  TimeSeries ts = tsSet.getTimeSeries(0);
  BOOST_TEST( ts[0] == 3.029296875 );
  BOOST_TEST( ts[ts.getLength() - 1] == 2.537109375);

  ts = tsSet.getTimeSeries(9);
  BOOST_TEST( ts[0] == 1.089843750 );
  BOOST_TEST( ts[ts.getLength() - 1] == 0.002822876);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_comma, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_15_20_comma, 20, ",", 0);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 15 );
  BOOST_CHECK( tsSet.getFilePath() == data.test_15_20_comma );
}

BOOST_AUTO_TEST_CASE( time_series_set_load_file_not_exist )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.not_exist, 0, " ", 0), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_uneven_rows )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.uneven_rows, 10, " ", 0), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_get_sub_time_series, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, " ", 0);

  TimeSeries ts = tsSet.getTimeSeries(0, 5, 10);
  BOOST_CHECK_EQUAL( ts.getLength(), 5);
  BOOST_TEST( ts[0] == 2.656250000);
  BOOST_TEST( ts[4] == 3.822265625);
  BOOST_CHECK_THROW( ts[5], GenexException );
}

BOOST_AUTO_TEST_CASE( time_series_set_get_invalid_sub_time_series)
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, " ", 0);

  BOOST_CHECK_THROW( tsSet.getTimeSeries(100), GenexException );        // index exceeds number of time series
  BOOST_CHECK_THROW( tsSet.getTimeSeries(-2), GenexException );         // negative index
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, -1, 10), GenexException );  // negative starting position
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 0, 100), GenexException );  // ending position exceeds time series length
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 11, 10), GenexException );  // starting position is larger than ending position
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 10, 10), GenexException );  // starting position is equal to ending position

}
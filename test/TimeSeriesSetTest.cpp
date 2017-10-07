#define BOOST_TEST_MODULE "Test TimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "TimeSeriesSet.hpp"
#include "Exception.hpp"
#include "TimeSeries.hpp"
#include "distance/Euclidean.hpp"

#include <iostream>

#define TOLERANCE 1e-9

using namespace genex;

struct MockDataset
{
  std::string test_10_20_space = "datasets/test/test_10_20_space.txt";
  std::string test_15_20_comma = "datasets/test/test_15_20_comma.csv";
  std::string not_exist = "unicorn_santa_magic_halting_problem_solution";
  std::string uneven_rows = "datasets/test/uneven_rows.txt";
  std::string text_only = "datasets/test/test_text_only.txt";
  std::string very_big = "datasets/test/very_big_value.txt";
  std::string test_5_10_space = "datasets/test/test_5_10_space.txt";
  std::string test_3_10_space = "datasets/test/test_3_10_space.txt";
  std::string test_3_11_space = "datasets/test/test_3_11_space.txt";
} data;

BOOST_AUTO_TEST_CASE( time_series_set_load_space, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, 0, " ");

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
  tsSet.loadData(data.test_15_20_comma, 20, 0, ",");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 15 );
  BOOST_CHECK( tsSet.getFilePath() == data.test_15_20_comma );

  TimeSeries ts = tsSet.getTimeSeries(0);
  BOOST_TEST( ts[0] == 4.122284712 );
  BOOST_TEST( ts[ts.getLength() - 1] == 2.684802835);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_file_not_exist )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.not_exist, 0, 0, " "), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_uneven_rows )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.uneven_rows, 10, 0, " "), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_text_only )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.text_only, 10, 0, " "), GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_set_load_very_big_value )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.loadData(data.very_big, 0, 0, " "), GenexException);
}

BOOST_AUTO_TEST_CASE( timeseries_set_load_all )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_15_20_comma, -1, 0, ",");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 15 );
  BOOST_CHECK( tsSet.getFilePath() == data.test_15_20_comma );
}

BOOST_AUTO_TEST_CASE( time_series_set_get_sub_time_series, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, 0, " ");

  TimeSeries ts = tsSet.getTimeSeries(0, 5, 10);
  BOOST_CHECK_EQUAL( ts.getLength(), 5);
  BOOST_TEST( ts[0] == 2.656250000);
  BOOST_TEST( ts[4] == 3.822265625);
  BOOST_CHECK_THROW( ts[5], GenexException );
}

BOOST_AUTO_TEST_CASE( time_series_set_get_invalid_sub_time_series)
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 20, 0, " ");

  BOOST_CHECK_THROW( tsSet.getTimeSeries(100), GenexException );        // index exceeds number of time series
  BOOST_CHECK_THROW( tsSet.getTimeSeries(-2), GenexException );         // negative index
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, -1, 10), GenexException );  // negative starting position
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 0, 100), GenexException );  // ending position exceeds time series length
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 11, 10), GenexException );  // starting position is larger than ending position
  BOOST_CHECK_THROW( tsSet.getTimeSeries(0, 10, 10), GenexException );  // starting position is equal to ending position
}

BOOST_AUTO_TEST_CASE( time_series_set_load_omit_rows_and_columns )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_10_20_space, 5, 5, " ");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 15 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 5 );

  TimeSeries ts = tsSet.getTimeSeries(0);
  BOOST_TEST( ts[0] == 2.656250000 );
  BOOST_TEST( ts[ts.getLength() - 1] == 2.537109375);
}

BOOST_AUTO_TEST_CASE( normalize, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_5_10_space, 10, 0, " ");
  std::pair<data_t, data_t> min_max = tsSet.normalize();
  BOOST_TEST( min_max.first == 1 );
  BOOST_TEST( min_max.second == 11 );

  TimeSeries t = tsSet.getTimeSeries(0);
  for(int i = 0; i < tsSet.getItemLength(); i++)
  {
    BOOST_TEST( t[i] == (i)/10.0 );
  }
}

BOOST_AUTO_TEST_CASE( normalize_2, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_11_space, 11, 0, " ");
  std::pair<data_t, data_t> min_max = tsSet.normalize();
  BOOST_TEST( min_max.first == 1 );
  BOOST_TEST( min_max.second == 21 );

  TimeSeries t = tsSet.getTimeSeries(0);
  for(int i = 0; i < tsSet.getItemLength(); i++)
  {
    BOOST_TEST( t[i] == (i)/20.0 );
  }

  TimeSeries t_2 = tsSet.getTimeSeries(2);
  for(int i = 0; i < tsSet.getItemLength(); i++)
  {
    BOOST_TEST( t_2[i] == (i + 10)/20.0 );
  }
}

BOOST_AUTO_TEST_CASE( normalize_exception )
{
  TimeSeriesSet tsSet;
  BOOST_CHECK_THROW(tsSet.normalize(), GenexException); // no data to normalize
}

BOOST_AUTO_TEST_CASE( get_distance_between, *boost::unit_test::tolerance(TOLERANCE) )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 10, 0, " ");
  data_t dist = tsSet.distanceBetween(0,0,10, tsSet.getTimeSeries(0), "euclidean");
  BOOST_TEST( dist == 0 );

  dist = tsSet.distanceBetween(1, 0, 10, tsSet.getTimeSeries(0), "euclidean");
  BOOST_TEST( dist == sqrt(1.0 / 10.0) );
}

BOOST_AUTO_TEST_CASE( basic_k_exhaustive )
{
  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, 20, 0, " ");
  std::vector<candidate_time_series_t> best = tsSet.kSimRaw(tsSet.getTimeSeries(0), 1);
  BOOST_TEST( best[0].dist == 0.0 );
}
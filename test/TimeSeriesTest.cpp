#define BOOST_TEST_MODULE "Test TimeSeries class"

#include <boost/test/unit_test.hpp>

#include "Exception.hpp"
#include "TimeSeries.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  data_t dat[7] = {1, 2, 3, 4, 5, 6, 7};
  data_t dat2[7] = {-1, 4, 5, 3, 1.5, 6, -5};
  data_t sum[7] = {0, 6, 8, 7, 6.5, 12, 2};
};

BOOST_AUTO_TEST_CASE( time_series_length )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  BOOST_CHECK_EQUAL( ts.getLength(), 5 );
}

BOOST_AUTO_TEST_CASE( time_series_out_of_range )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  BOOST_CHECK_THROW( ts[10], GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_modify_data )
{
  MockData data;
  TimeSeries ts(data.dat, 2);
  BOOST_CHECK_EQUAL( ts[0], 1 );
  ts[0] = 99;
  BOOST_CHECK_EQUAL( ts[0], 99 );
}

BOOST_AUTO_TEST_CASE( time_series_addition, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts1(data.dat, 7);
  TimeSeries ts2(data.dat2, 7);
  ts1 += ts2;
  for (int i = 0; i < ts1.getLength(); i++)
  {
    BOOST_TEST( ts1[i] == data.sum[i] );
  }
}

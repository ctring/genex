#define BOOST_TEST_MODULE "Test TimeSeries class"

#include <boost/test/unit_test.hpp>

#include "Exception.hpp"
#include "TimeSeries.hpp"

using namespace genex;

struct MockData
{
  data_t dat[7] = {1, 2, 3, 4, 5, 6, 7};
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

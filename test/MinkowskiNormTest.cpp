#define BOOST_TEST_MODULE "Test Minkowski Normalization class"

#include <boost/test/unit_test.hpp>
#include "Exception.hpp"
#include "Minkowski.hpp"

struct MockData
{
  data_t dat[7] = {1, 2, 3, 4, 5, 6, 7};
};

BOOST_AUTO_TEST_CASE( time_series_length )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  Minkowski dist;

  BOOST_CHECK_EQUAL( dist.norm(ts), 5 );
}


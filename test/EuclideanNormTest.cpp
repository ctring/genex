#define BOOST_TEST_MODULE "Test Euclidean Normalization class"

#include <boost/test/unit_test.hpp>
#include "Euclidean.hpp"

struct MockData
{
  data_t dat[7] = {1, 2, 3, 4, 5, 6, 7};
};

BOOST_AUTO_TEST_CASE( time_series_length )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  Euclidean dist;

  BOOST_CHECK_EQUAL( dist.norm(ts), 5 );
}


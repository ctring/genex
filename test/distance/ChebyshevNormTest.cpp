#define BOOST_TEST_MODULE "Test Chebyshev Normalization class"

#include <boost/test/unit_test.hpp>

#include "distance/Chebyshev.hpp"

using namespace genex;

#define TOLERANCE 1e-9

struct MockData
{
  data_t dat_1[5] = {1, 2, 3, 4, 5};
  data_t dat_2[5] = {11, 2, 3, 4, 5};
};

BOOST_AUTO_TEST_CASE( time_series_length, *boost::unit_test::tolerance(TOLERANCE)  )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);
  Chebyshev dist;

  data_t* total = dist.init();

  for (int i = 0; i < ts_1.getLength(); i++) {
    dist.reduce(total, total, ts_1[i], ts_2[i]);
  }

  BOOST_TEST( dist.norm(total, ts_1, ts_2), 10.0 );

  delete total;
}

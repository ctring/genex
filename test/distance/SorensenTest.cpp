#define BOOST_TEST_MODULE "Test Sorensen class"

#include <boost/test/included/unit_test.hpp>

#include "distance/Sorensen.hpp"

using namespace genex;

#define TOLERANCE 1e-9

struct MockData
{
  data_t dat_1[5] = {3, 1, 2, 5, 4};
  data_t dat_2[5] = {3.4, 4, 1.2, 2, 3};
};

BOOST_AUTO_TEST_CASE( sorensen_test, *boost::unit_test::tolerance(TOLERANCE)  )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 5);
  TimeSeries ts_2(data.dat_2, 5);
  Sorensen dist;

  data_t* total = dist.init();

  for (int i = 0; i < ts_1.getLength(); i++) {
    total = dist.reduce(total, total, ts_1[i], ts_2[i]);
  }

  BOOST_TEST( dist.norm(total, ts_1, ts_2), 0.3216 );

  delete total;
}
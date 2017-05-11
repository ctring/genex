#define BOOST_TEST_MODULE "Test Cosine class"

#include <boost/test/unit_test.hpp>

#include "distance/Cosine.hpp"

using namespace genex;

#define TOLERANCE 1e-9

struct MockData
{
  data_t dat_1[10] = {5, 0, 3, 0, 2, 0, 0, 2, 0, 0};
  data_t dat_2[10] = {3, 0, 2, 0, 1, 1, 0, 1, 0, 1};
};

BOOST_AUTO_TEST_CASE( cosine_test, *boost::unit_test::tolerance(TOLERANCE)  )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);
  Cosine dist;

  data_t* total = dist.init();

  for (int i = 0; i < ts_1.getLength(); i++) {
    total = dist.reduce(total, total, ts_1[i], ts_2[i]);
  }

  BOOST_TEST( dist.norm(total, ts_1, ts_2), 0.94 );

  delete total;
}

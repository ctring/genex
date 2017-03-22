#define BOOST_TEST_MODULE "Test General Distance Function"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"
#include "distance/Chebyshev.hpp"
#include "distance/Manhattan.hpp"

#include "distance/Distance.hpp"

using namespace genex;

#define TOLERANCE 1e-9
struct MockData
{
  data_t dat_1[5] = {1, 2, 3, 4, 5};
  data_t dat_2[5] = {11, 2, 3, 4, 5};
};

BOOST_AUTO_TEST_CASE( time_series_length, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);
  Euclidean dist_1;
  Manhattan dist_2;
  Chebyshev dist_3;

  data_t total_1 = generalDistance(dist_1, ts_1, ts_2, 5);
  BOOST_TEST( total_1, 2.0 );

  data_t total_2 = generalDistance(dist_2, ts_1, ts_2, 5);
  BOOST_TEST( total_2, 2.0 );

  data_t total_3 = generalDistance(dist_3, ts_1, ts_2, 5);
  BOOST_TEST( total_3, 10.0 );
}

#define BOOST_TEST_MODULE "Testing Euclidean Functions"

#include <boost/test/included/unit_test.hpp>

#include "distance/Euclidean.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( eucdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Euclidean d;

   TimeSeries ts_1 {NULL, 0, 0, 2};
   TimeSeries ts_2 {NULL, 0, 0, 2};

   data_t a = d.dist(100.0, 110.0);

   data_t first = d.init();
   data_t second = d.reduce(first, first, 4.0, 0.0);
   data_t third = d.reduce(second, second, 7.0, 10.0);
   data_t c = d.norm(third, ts_1, ts_2);

   d.clean(third);

   BOOST_TEST( a == 100 );
   BOOST_TEST( c == sqrt(25.0 / 2) );
}

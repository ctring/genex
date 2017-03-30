#define BOOST_TEST_MODULE "Testing Euclidean Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( eucdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Euclidean d;

   TimeSeries ts_1 {NULL, 0, 0, 2};
   TimeSeries ts_2 {NULL, 0, 0, 2};

   data_t a = d.dist(100.0, 110.0);

   Cache* first = d.init();
   Cache* second = d.reduce(first, 4.0, 0.0);
   Cache* third = d.reduce(second, 7.0, 10.0);
   data_t c = d.norm(third, ts_1, ts_2);

   delete first;
   delete second;
   delete third;

   BOOST_TEST( a == 100 );
   BOOST_TEST( c == 2.5 );
}

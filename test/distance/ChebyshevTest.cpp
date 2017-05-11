#define BOOST_TEST_MODULE "Testing Chebyshev Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Chebyshev.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( chebdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Chebyshev d;
   data_t a = d.dist(100.0, 110.0);

   TimeSeries ts_1 {NULL, 0, 0, 2};
   TimeSeries ts_2 {NULL, 0, 0, 2};

   data_t* first = d.init(); //-INF
   data_t* second = d.reduce(first, first, 50.0, 0);
   data_t* third = d.reduce(second, second, 20.0, 10.0);
   data_t c = d.norm(third, ts_1, ts_2);

   delete third;

   BOOST_TEST( a == 10 );
   BOOST_TEST( c == 50 );
}

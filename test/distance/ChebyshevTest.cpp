#define BOOST_TEST_MODULE "Testing Chebyshev Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Chebyshev.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( chebdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Chebyshev d;
   data_t a = d.dist(100.0, 110.0);
   data_t b = d.reduce(50.0, 20.0, 10.0);

   BOOST_TEST( a == 10 );
   BOOST_TEST( b == 50 );
}

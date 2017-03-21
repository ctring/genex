#define BOOST_TEST_MODULE "Testing Euclidean Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( eucdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Euclidean d;
   data_t a = d.dist(100.0, 110.0);
   data_t b = d.reduce(50.0, 20.0, 10.0);

   BOOST_TEST( a == 100 );
   BOOST_TEST( b == 150 );
}

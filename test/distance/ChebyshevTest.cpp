#define BOOST_TEST_MODULE "Testing Chebyshev Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Chebyshev.hpp"


BOOST_AUTO_TEST_CASE( chebdist )
{
   Chebyshev d;
   data_t a = d.dist(100.0, 110.0);
   data_t b = d.recurse(50.0, 20.0, 10.0);

   BOOST_CHECK( a == 10 );
   BOOST_CHECK( b == 50 );
}


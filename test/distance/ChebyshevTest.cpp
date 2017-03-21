#define BOOST_TEST_MODULE "Testing Chebyshev Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Chebyshev.hpp"

<<<<<<< a9f0d1534786f6000974d6945c3b24ca1f9cbcb8:test/distance/ChebyshevTest.cpp
using namespace genex;
=======
#define TOLERANCE 1e-9
>>>>>>> Tolerance & Reduce:test/ChebyshevTest.cpp

BOOST_AUTO_TEST_CASE( chebdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Chebyshev d;
   data_t a = d.dist(100.0, 110.0);
<<<<<<< a9f0d1534786f6000974d6945c3b24ca1f9cbcb8:test/distance/ChebyshevTest.cpp
   data_t b = d.recurse(50.0, 20.0, 10.0);

   BOOST_CHECK( a == 10 );
   BOOST_CHECK( b == 50 );
}
=======
   data_t b = d.reduce(50.0, 20.0, 10.0);
>>>>>>> Tolerance & Reduce:test/ChebyshevTest.cpp

   BOOST_TEST( a == 10 );
   BOOST_TEST( b == 50 );
}

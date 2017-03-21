#define BOOST_TEST_MODULE "Testing Euclidean Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Euclidean.hpp"

<<<<<<< a9f0d1534786f6000974d6945c3b24ca1f9cbcb8:test/distance/EuclideanTest.cpp
using namespace genex;
=======
#define TOLERANCE 1e-9
>>>>>>> Tolerance & Reduce:test/EuclideanTest.cpp

BOOST_AUTO_TEST_CASE( eucdist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Euclidean d;
   data_t a = d.dist(100.0, 110.0);
<<<<<<< a9f0d1534786f6000974d6945c3b24ca1f9cbcb8:test/distance/EuclideanTest.cpp
   data_t b = d.recurse(50.0, 20.0, 10.0);

   BOOST_CHECK( a == 100 );
   BOOST_CHECK( b == 150 );
}
=======
   data_t b = d.reduce(50.0, 20.0, 10.0);
>>>>>>> Tolerance & Reduce:test/EuclideanTest.cpp

   BOOST_TEST( a == 100 );
   BOOST_TEST( b == 150 );
}

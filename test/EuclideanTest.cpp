#define BOOST_TEST_MODULE "Testing Euclidean Functions"

#include <boost/test/unit_test.hpp>

#include "Euclidean.hpp"


BOOST_AUTO_TEST_CASE( eucdist )
{
   Euclidean d;
   data_t a = d.dist(100.0, 110.0);
   data_t b = d.recurse(50.0, 20.0, 10.0);
  
   BOOST_CHECK( a == 100 );
   BOOST_CHECK( b == 150 );
}


#define BOOST_TEST_MODULE "Testing Manhattan Functions"

#include <boost/test/unit_test.hpp>

#include "Manhattan.hpp"


BOOST_AUTO_TEST_CASE( mandist )
{
   Manhattan d;
   data_t a = d.dist(100.0, 110.0);
   data_t b = d.recurse(50.0, 60.0, 10.0);
   
   BOOST_CHECK( a == 10 );
   BOOST_CHECK( b == 100 );
}


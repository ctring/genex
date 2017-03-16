// define the module test name (the class name)
#define BOOST_TEST_MODULE "BaseClassModule"

#include <boost/test/unit_test.hpp>

//#include "some_project/some_base_class.h"
// example function to test:
int add( int i, int j ) { return i+j; }

BOOST_AUTO_TEST_CASE( basicMath )
{
   BOOST_CHECK( add( 2,2 ) == 4 );
   BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
}


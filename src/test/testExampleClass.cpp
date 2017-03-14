#define BOOST_TEST_DYN_LINK

// define the module test name (generally the class name)
#define BOOST_TEST_MODULE "Testing including classes"

#include <boost/test/unit_test.hpp>

#include "ExampleClass.hpp"

// example function to test:
int add( int i, int j ) { return i+j; }

BOOST_AUTO_TEST_CASE( basicClass )
{
   Rectangle rect;
   rect.set_values (3,4);

   BOOST_CHECK( rect.area() == 12 );
}


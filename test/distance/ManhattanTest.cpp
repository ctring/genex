#define BOOST_TEST_MODULE "Testing Manhattan Functions"

#include <boost/test/unit_test.hpp>

#include "distance/Manhattan.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( mandist, *boost::unit_test::tolerance(TOLERANCE) )
{
   Manhattan d;

   TimeSeries ts_1 {NULL, 0, 0, 2};
   TimeSeries ts_2 {NULL, 0, 0, 2};

   data_t a = d.dist(100.0, 110.0);
   data_t* prev = d.init(); //0
   data_t* first  = d.reduce(prev, prev, 60.0, 10.0); //50
   data_t* second = d.reduce(first, first, 15.0, 5.0);
   data_t c = d.norm(second, ts_1, ts_2);

   delete second;

   BOOST_TEST( a == 10 );
   BOOST_TEST( c == 30 );
}

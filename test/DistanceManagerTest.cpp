#define BOOST_TEST_MODULE "Testing Distance Manager"

#include <boost/test/unit_test.hpp>

#include "DistanceManager.hpp"


BOOST_AUTO_TEST_CASE( eucdist )
{
   DistanceManager m;
   DistanceMetric * d = m.getMetric(Dists::EUC);
  
   data_t a = d->dist(100.0, 110.0);
   data_t b = d->recurse(50.0, 20.0, 10.0);
   
   BOOST_CHECK( a == 100 );
   BOOST_CHECK( b == 150 );

   DistanceMetric * d_2 = m.getMetric(Dists::MINK);
  
   data_t e = d_2->dist(100.0, 110.0);
   data_t f = d_2->recurse(50.0, 20.0, 10.0);
   
   BOOST_CHECK( e == 10 );
   BOOST_CHECK( f == 60 );

}


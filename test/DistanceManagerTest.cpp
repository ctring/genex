#define BOOST_TEST_MODULE "Testing Distance Manager"

#include <boost/test/unit_test.hpp>

#include "DistanceManager.hpp"
#include "Exception.hpp"


BOOST_AUTO_TEST_CASE( distance_manager )
{
   const DistanceManager* m = DistanceManager::getInstance();
   DistanceMetric * d = m->getMetric("euclidean");

   data_t a = d->dist(100.0, 110.0);
   data_t b = d->recurse(50.0, 20.0, 10.0);

   BOOST_CHECK( a == 100 );
   BOOST_CHECK( b == 150 );

   DistanceMetric * d_2 = m->getMetric("manhattan");

   data_t e = d_2->dist(100.0, 110.0);
   data_t f = d_2->recurse(50.0, 20.0, 10.0);

   BOOST_CHECK( e == 10 );
   BOOST_CHECK( f == 60 );
}

BOOST_AUTO_TEST_CASE( distance_manager_not_found )
{
   const DistanceManager* m = DistanceManager::getInstance();
   BOOST_CHECK_THROW( m->getMetric("oracle"), GenexException);
}


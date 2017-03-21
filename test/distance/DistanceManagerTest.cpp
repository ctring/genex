#define BOOST_TEST_MODULE "Testing Distance Manager"

#include <boost/test/unit_test.hpp>

#include "distance/DistanceManager.hpp"
#include "Exception.hpp"

using namespace genex;
#define TOLERANCE 1e-9

BOOST_AUTO_TEST_CASE( distance_manager, *boost::unit_test::tolerance(TOLERANCE) )
{
   const DistanceManager* m = DistanceManager::getInstance();
   DistanceMetric * d = m->getMetric("euclidean");

   data_t a = d->dist(100.0, 110.0);
   data_t b = d->reduce(50.0, 20.0, 10.0);

   BOOST_TEST( a == 100 );
   BOOST_TEST( b == 150 );

   DistanceMetric * d_2 = m->getMetric("manhattan");

   data_t e = d_2->dist(100.0, 110.0);
   data_t f = d_2->reduce(50.0, 20.0, 10.0);

   BOOST_TEST( e == 10 );
   BOOST_TEST( f == 60 );
}

BOOST_AUTO_TEST_CASE( distance_manager_not_found )
{
   const DistanceManager* m = DistanceManager::getInstance();
   BOOST_CHECK_THROW( m->getMetric("oracle"), GenexException);
}

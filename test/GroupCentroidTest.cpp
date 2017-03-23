#define BOOST_TEST_MODULE "Test GroupCentroid class"

#include <boost/test/unit_test.hpp>
#include <iostream> // debug
#include "GroupCentroid.hpp"

#define TOLERANCE 1e-9

using namespace genex;
using namespace std;
struct MockData
{
  data_t dat_1[7] = {1, 2, 3, 4, 5, 6, 7};
  data_t dat_2[7] = {5, 4, 3, 2, 1, 6, 7};
  data_t dat_3[5] = {6, 6, 6, 6, 6};
};

BOOST_AUTO_TEST_CASE( group_centroid_time_series, *boost::unit_test::tolerance(TOLERANCE) )
{
  MockData data;
  GroupCentroid gc(5);

  BOOST_CHECK_EQUAL( gc.getLength(), 5 );
  BOOST_CHECK_EQUAL( gc.getCount(), 0 );

  TimeSeries ts_1(data.dat_1, 0, 0, 5);
  TimeSeries ts_2(data.dat_2, 0, 0, 5);
  TimeSeries ts_3(data.dat_3, 0, 0, 5);

  gc.addTimeSeries(ts_1);
  gc.addTimeSeries(ts_2);

  for(int i = 0; i < 5; i++)
  {
    BOOST_TEST( gc[i] == 3.0 );
  }

  //test that it does not mutate original
  for(int i = 0; i < 5; i++)
  {
    BOOST_TEST( gc.getSumValue(i) == 6.0 );
  }

  //test that it updates
  gc.addTimeSeries(ts_3);
  for(int i = 0; i < 5; i++)
  {
    BOOST_TEST( gc[i] == 4.0 );
  }
}

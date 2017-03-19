#define BOOST_TEST_MODULE "Test GroupableTimeSeriesSet class"

#include <boost/test/unit_test.hpp>

#include "GroupableTimeSeriesSet.hpp"
#include "Exception.hpp"

struct MockDataset
{
  std::string test_10_20_space = "dataset/test/test_10_20_space.txt";
} data;

BOOST_AUTO_TEST_CASE( groupable_time_series_set_load_space )
{
  GroupableTimeSeriesSet tsSet("test_10_20_space");
  tsSet.loadData(data.test_10_20_space, 20);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), 20 );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), 10 );
  BOOST_CHECK( tsSet.getName() == std::string("test_10_20_space") );
}

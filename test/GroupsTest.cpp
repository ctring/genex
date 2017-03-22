#define BOOST_TEST_MODULE "Test Groups class"

#include <boost/test/unit_test.hpp>

#include "Exception.hpp"
#include "Group.hpp"

#define TOLERANCE 1e-9

using namespace genex;

struct MockData
{
  data_t dat_1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  data_t dat_2[10] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
  data_t dat_3[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  data_t dat_4[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  data_t dat_5[10] = {9, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  std::string test_5_10_space = "dataset/test/test_5_10_space.txt";
};

BOOST_AUTO_TEST_CASE( basic_groups )
{
  MockData data;

  int timeSeriesCount = 5;
  int timeSeriesLengths = 10;
  int memberLength = 5;
  int repLength = timeSeriesLengths - memberLength + 1;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_5_10_space, timeSeriesCount, " ", 0);

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), timeSeriesLengths );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), timeSeriesCount );
  BOOST_CHECK( tsSet.getFilePath() == data.test_5_10_space );
  node_t* memberMap = new node_t[timeSeriesCount * repLength];

  Group g();
}

#define BOOST_TEST_MODULE "Test Groups class"

#include <boost/test/unit_test.hpp>
#include <cstdio>
#include "IO.hpp"
#include "distance/Euclidean.hpp"
#include "distance/Distance.hpp"
#include "Exception.hpp"
#include "Group.hpp"

using namespace genex;

struct MockData
{
  data_t dat_1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  data_t dat_2[10] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5};

  data_t centroid_1[5] = {1, 1.5, 2.5, 3, 4};
  data_t centroid_2[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.5};

  data_t dat_3[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  data_t dat_4[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  data_t dat_5[10] = {9, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  data_t dat_sim_1[5] = {1, 2, 3, 4, 5};
  data_t dat_sim_2[5] = {2, 3, 4, 5, 6};
  data_t dat_diff[5]  = {12, 13, 14, 15};

  std::string test_5_10_space = "datasets/test/test_5_10_space.txt";
  std::string test_3_10_space = "datasets/test/test_3_10_space.txt";
};

BOOST_AUTO_TEST_CASE( basic_groups, *boost::unit_test::tolerance(EPS) )
{
  MockData data;

  int timeSeriesCount = 5;
  int timeSeriesLengths = 10;
  int memberLength = 5;
  int subTimeSeriesCount = timeSeriesLengths - memberLength + 1;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_5_10_space, timeSeriesCount, 0, " ");

  BOOST_CHECK_EQUAL( tsSet.getItemLength(), timeSeriesLengths );
  BOOST_CHECK_EQUAL( tsSet.getItemCount(), timeSeriesCount );
  BOOST_CHECK( tsSet.getFilePath() == data.test_5_10_space );

  std::vector<group_membership_t> memberMap =
    std::vector<group_membership_t>(tsSet.getItemCount() * tsSet.getItemLength());
  Group g(0, memberLength, subTimeSeriesCount, tsSet, memberMap);

  const TimeSeries& c = g.getCentroid();

  // test initial centroid is all zeros
  for (int i = 0; i < memberLength; i++)
  {
    BOOST_TEST( c[i] == 0 );
  }

  g.addMember(0, 0);
  g.setCentroid(0, 0);

  //checking if centroid is updated
  BOOST_CHECK_EQUAL( g.getCount(), 1 );
  for (int i = 0; i < memberLength; i++)
  {
    BOOST_TEST( c[i] == data.dat_1[i] );
  }

  g.addMember(1, 0);

  BOOST_CHECK_EQUAL( g.getCount(), 2 );
}

BOOST_AUTO_TEST_CASE( group_get_best_match, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  dist_t distance = warpedDistance<Euclidean, data_t>;

  int timeSeriesCount = 3;
  int timeSeriesLengths = 10;
  int memberLength = 10;
  int subTimeSeriesCount = timeSeriesLengths - memberLength + 1;

  TimeSeriesSet tsSet;
  tsSet.loadData(data.test_3_10_space, timeSeriesCount, 0, " ");

  std::vector<group_membership_t> memberMap =
    std::vector<group_membership_t>(tsSet.getItemCount() * tsSet.getItemLength());


  Group g(0, memberLength, subTimeSeriesCount, tsSet, memberMap);
  g.addMember(2, 0);
  g.addMember(0, 0);
  TimeSeries t = tsSet.getTimeSeries(1,0,memberLength);
  BOOST_TEST(t[0] == 1.0);
  candidate_time_series_t best = g.getBestMatch(t, distance);
  BOOST_TEST(best.dist == sqrt(1.0)/(2 * 10.0));
}

BOOST_AUTO_TEST_CASE( group_save_and_load , *boost::unit_test::tolerance(EPS) )
{
  // Set up  
  std::string fname = "group_save_and_load.z";
  MockData data;
  auto dataset = TimeSeriesSet();
  dataset.loadData(data.test_3_10_space, 0, 0, " ");
  int length = 4;
  int subTimeSeriesCount = dataset.getItemLength() - length + 1;
  auto memberMap = std::vector<group_membership_t>(dataset.getItemCount() * subTimeSeriesCount);
  auto group = new Group(1, length, subTimeSeriesCount, dataset, memberMap);
  group->setCentroid(1, 2);
  group->addMember(0, 0);
  group->addMember(2, 4);

  // Save
  saveToFile(*group, fname);
  
  // Load
  auto memberMap2 = std::vector<group_membership_t>(dataset.getItemCount() * subTimeSeriesCount);  
  auto group2 = new Group(1, length, subTimeSeriesCount, dataset, memberMap2);
  loadFromFile(*group2, fname);

  // Compare
  BOOST_CHECK_EQUAL( group->getCount(), group2->getCount() );
  BOOST_CHECK_EQUAL( group->getMemberLength(), group2->getMemberLength() );
  BOOST_CHECK_EQUAL( group->getCentroid(), group2->getCentroid() );

  for (auto ts : group->getMembers()) {
    bool ok = false;
    for (auto ts2 : group2->getMembers()) {
      if (ts == ts2) {
        ok = true;
        break;
      }
    }
    BOOST_TEST( ok );
  }
  remove(fname.c_str());
  delete group;
  delete group2;
}

#define BOOST_TEST_MODULE "Test TimeSeries class"

#include <boost/test/unit_test.hpp>
#include <cstdio>
#include "IO.hpp"
#include "Exception.hpp"
#include "TimeSeries.hpp"

using namespace genex;

struct MockData
{
  data_t dat[7] = {1, 2, 3, 4, 5, 6, 7};
  data_t dat2[7] = {-1, 4, 5, 3, 1.5, 6, -5};
  data_t sum[7] = {0, 6, 8, 7, 6.5, 12, 2};
  data_t dat3[10] = {0, 2, 3, 5, 8, 6, 3, 2, 3, 5};  

  data_t dat2Upper3[7] = {4, 5, 5, 5, 6, 6, 6};
  data_t dat2Lower3[7] = {-1, -1, 3, 1.5, 1.5, -5, -5};
  
  data_t dat2Upper5[7] = {5, 5, 5, 6, 6, 6, 6};
  data_t dat2Lower5[7] = {-1, -1, -1, 1.5, -5, -5, -5};

  data_t dat2Upper7[7] = {5, 5, 6, 6, 6, 6, 6};
  data_t dat2Lower7[7] = {-1, -1, -1, -5, -5, -5, -5};

  data_t dat2Upper9[7] = {5, 6, 6, 6, 6, 6, 6};
  data_t dat2Lower9[7] = {-1, -1, -5, -5, -5, -5, -5};

  data_t dat2Upper13[7] = {6, 6, 6, 6, 6, 6, 6};
  data_t dat2Lower13[7] = {-5, -5, -5, -5, -5, -5, -5};

  data_t dat3Upper5[10] = {3, 5, 8, 8, 8, 8, 8, 6, 5, 5};
  data_t dat3Lower5[10] = {0, 0, 0, 2, 3, 2, 2, 2, 2, 2};
  
};

BOOST_AUTO_TEST_CASE( time_series_length )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  BOOST_CHECK_EQUAL( ts.getLength(), 5 );
}

BOOST_AUTO_TEST_CASE( time_series_out_of_range )
{
  MockData data;
  TimeSeries ts(data.dat, 0, 0, 5);
  BOOST_CHECK_THROW( ts[10], GenexException);
}

BOOST_AUTO_TEST_CASE( time_series_modify_data )
{
  MockData data;
  TimeSeries ts(data.dat, 2);
  BOOST_CHECK_EQUAL( ts[0], 1 );
  ts[0] = 99;
  BOOST_CHECK_EQUAL( ts[0], 99 );
}

BOOST_AUTO_TEST_CASE( time_series_addition, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  TimeSeries ts1(data.dat, 7);
  TimeSeries ts2(data.dat2, 7);
  ts1 += ts2;
  for (int i = 0; i < ts1.getLength(); i++)
  {
    BOOST_TEST( ts1[i] == data.sum[i] );
  }
}

BOOST_AUTO_TEST_CASE( time_series_keogh_upper_lower, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  TimeSeries ts(data.dat2, 7);
  TimeSeries ts2(data.dat3, 10);

  for (int i = 0; i < ts.getLength(); i++)
  {
    BOOST_TEST( data.dat2Upper3[i] == ts.getKeoghUpper(1)[i] );
    BOOST_TEST( data.dat2Lower3[i] == ts.getKeoghLower(1)[i] );
  }

  for (int i = 0; i < ts.getLength(); i++)
  {
    BOOST_TEST( data.dat2Upper5[i] == ts.getKeoghUpper(2)[i] );
    BOOST_TEST( data.dat2Lower5[i] == ts.getKeoghLower(2)[i] );
  }

  for (int i = 0; i < ts.getLength(); i++)
  {
    BOOST_TEST( data.dat2Upper7[i] == ts.getKeoghUpper(3)[i] );
    BOOST_TEST( data.dat2Lower7[i] == ts.getKeoghLower(3)[i] );
  }

  for (int i = 0; i < ts.getLength(); i++)
  {
    BOOST_TEST( data.dat2Upper9[i] == ts.getKeoghUpper(4)[i] );
    BOOST_TEST( data.dat2Lower9[i] == ts.getKeoghLower(4)[i] );
  }

  for (int i = 0; i < ts.getLength(); i++)
  {
    BOOST_TEST( data.dat2Upper13[i] == ts.getKeoghUpper(6)[i] );
    BOOST_TEST( data.dat2Lower13[i] == ts.getKeoghLower(6)[i] );
  }

  for (int i = 0; i < ts2.getLength(); i++)
  {
    BOOST_TEST( data.dat3Upper5[i] == ts2.getKeoghUpper(2)[i] );
    BOOST_TEST( data.dat3Lower5[i] == ts2.getKeoghLower(2)[i] );    
  }
}

BOOST_AUTO_TEST_CASE( time_series_save_and_load, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  const std::string fname = "test_time_series_save_and_load.z";
  TimeSeries ts(data.dat, 7);
  saveToFile(ts, fname);

  TimeSeries ts2(7);
  loadFromFile(ts2, fname);
  BOOST_CHECK_EQUAL( ts.getLength(), ts2.getLength() );
  for (int i = 0; i < ts.getLength(); i++) {
    BOOST_TEST( ts[i] == ts2[i] );
  }
  remove(fname.c_str());
}

BOOST_AUTO_TEST_CASE( time_series_check_equal, *boost::unit_test::tolerance(EPS) )
{
  MockData data;
  TimeSeries ts1(data.dat, 7);
  TimeSeries ts2(data.dat, 7);
  TimeSeries ts3(7);
}
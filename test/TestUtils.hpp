#include <boost/test/unit_test.hpp>
#include <vector>
#include <TimeSeries.hpp>

using std::vector;
using genex::TimeSeries;
using genex::data_t;

inline void boostCheckTimeSeries(const TimeSeries& ts, const vector<data_t> actual)
{
    BOOST_REQUIRE_EQUAL( ts.getLength(), actual.size() );
    for (int i = 0; i < ts.getLength(); i++) {
        BOOST_TEST( ts[i] == actual[i] );
    }
}
#include <catch2/catch_test_macros.hpp>
#include <util/hashing.hpp>
#include <util/monitoring/stats.hpp>

TEST_CASE( "Util::Monitoring::Stats", "[unit]" )
{
	using namespace VTX::Util;
	using namespace Monitoring;

	Stats stats;

	FrameInfo & frame1 = stats.newFrame();
	frame1.set<int>( hash( "metric1" ), 10 );
	frame1.set<float>( hash( "metric2" ), 20.f );

	FrameInfo & frame2 = stats.newFrame();
	frame1.set<int>( hash( "metric1" ), 20 );
	frame1.set<float>( hash( "metric2" ), 80.f );

	int averageMetric1 = stats.average<int>( hash( "metric1" ) );
	CHECK( averageMetric1 == 15 );
	float averageMetric2 = stats.average<float>( hash( "metric2" ) );
	CHECK( averageMetric2 == 40.f );
}

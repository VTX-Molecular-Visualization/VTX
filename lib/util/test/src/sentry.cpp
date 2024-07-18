#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <util/sentry.hpp>

TEST_CASE( "Util::Sentry", "[sentry]" )
{
	using namespace VTX::Util;

	SentryTarget t1;
	auto		 sentry1 = t1.newSentry();
	CHECK( sentry1 );

	std::optional<SentryTarget> t2 = std::nullopt;
	t2.emplace();
	auto sentry2 = t2->newSentry();
	CHECK( sentry2 );
	t2.reset();
	CHECK( not sentry2 );
}

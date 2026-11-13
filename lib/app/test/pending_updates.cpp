#include <app/fixture.hpp>
#include <app/input/input_manager.hpp>
#include <app/pass/pass_manager.hpp>
#include <app/pending_updates.hpp>
#include <app/services.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <renderer/renderer.hpp>
#include <util/enum.hpp>
#include <util/event_hub.hpp>

namespace
{
	struct PendingUpdatesTestEvent
	{
	};

	class CountingPass : public VTX::App::Pass::IPass
	{
	  public:
		static constexpr VTX::App::E_PENDING_UPDATE UPDATE_TRIGGERS = VTX::App::E_PENDING_UPDATE::PASSES;

		void update( const float, const float ) { ++count; }

		int count = 0;
	};
} // namespace

TEST_CASE( "VTX_APP - PendingUpdates - startup reaches idle", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Fixture app;

	CHECK( app.runUntilIdle() );
	CHECK( not PENDING_UPDATES().any() );
}

TEST_CASE( "VTX_APP - PendingUpdates - producers raise their bits", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Fixture app;
	REQUIRE( app.runUntilIdle() );

	HUB().enqueue<PendingUpdatesTestEvent>();
	CHECK( Util::Enum::hasAnyBit( PENDING_UPDATES().consume(), E_PENDING_UPDATE::EVENTS ) );

	INPUT().zoomBy( 1 );
	CHECK( Util::Enum::hasAnyBit( PENDING_UPDATES().consume(), E_PENDING_UPDATE::INPUT ) );
	INPUT().consume();

	RENDERER().setNeedUpdate();
	CHECK( Util::Enum::hasAnyBit( PENDING_UPDATES().consume(), E_PENDING_UPDATE::RENDER ) );

	CHECK( app.runUntilIdle() );
}

TEST_CASE( "VTX_APP - PendingUpdates - passes filtered by triggers", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Fixture app;
	REQUIRE( app.runUntilIdle() );

	CountingPass * const pass = PASS().addPass<CountingPass>();
	REQUIRE( pass != nullptr );

	PASS().update( E_PENDING_UPDATE::INPUT | E_PENDING_UPDATE::RENDER, 0.f, 0.f );
	CHECK( pass->count == 0 );

	PASS().update( E_PENDING_UPDATE::PASSES, 0.f, 0.f );
	CHECK( pass->count == 1 );

	PASS().update( 0.f, 0.f );
	CHECK( pass->count == 2 );

	PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
	CHECK( app.runUntilIdle() );
	CHECK( pass->count == 3 );

	PENDING_UPDATES().mark( E_PENDING_UPDATE::EVENTS );
	CHECK( app.runUntilIdle() );
	CHECK( pass->count == 3 );

	PASS().removePass<CountingPass>();
}

TEST_CASE( "VTX_APP - PendingUpdates - runUntil timeout", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Fixture app;
	REQUIRE( app.runUntilIdle() );

	CHECK( not app.runUntil( []() { return false; }, std::chrono::milliseconds( 10 ) ) );
	CHECK( app.runUntil( []() { return true; }, std::chrono::milliseconds( 10 ) ) );
}

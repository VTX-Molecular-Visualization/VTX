#include <catch2/catch_test_macros.hpp>
#include <util/players.hpp>
TEST_CASE( "Util::Players", "[players][forward]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	SECTION( "a 1 sized player" )
	{
		Player p( Players::Forward( 1 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
	}

	SECTION( "a 3 sized player" )
	{
		Player p( Players::Forward( 3 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 1 );
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 2 );
		p.increment();
		p.next( step );
		CHECK( step == 2 );
	}

	SECTION( "jumping" )
	{
		Player p( Players::Forward( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 1 );
		p.jumpTo( 5 );
		p.next( step );
		CHECK( step == 6 );
		p.jumpTo( 10000 );
		p.next( step );
		CHECK( step == 9 );
	}
}

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

TEST_CASE( "Util::Players", "[players][pingpong]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	SECTION( "a 1 sized player" )
	{
		Player p( Players::PingPong( 1 ) );

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
		Player p( Players::PingPong( 3 ) );

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
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 2 );
		p.increment();
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 2 );
	}

	SECTION( "increment N" )
	{
		{
			Player p( Players::PingPong( 10 ) );
			uint   step = 0;
			p.increment( 3 );
			p.current( step );
			CHECK( step == 3 );
			p.increment( 11 );
			p.current( step );
			CHECK( step == 4 );
			p.increment( 3 );
			p.current( step );
			CHECK( step == 1 );
			p.increment( 3 );
			p.current( step );
			CHECK( step == 2 );
			p.jumpTo( 5 );
			p.increment( 30 );
			p.current( step );
			CHECK( step == 1 );
			p.increment( 18 );
			p.current( step );
			CHECK( step == 1 );
		}
		{
			Player p( Players::PingPong( 10 ) );
			uint   step = 0;
			p.increment( 2 );
			p.increment( 18 );
			p.current( step );
			CHECK( step == 2 );
			p.increment( 1 );
			p.current( step );
			CHECK( step == 3 );
		}
	}

	SECTION( "jumping" )
	{
		Player p( Players::PingPong( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 1 );
		p.jumpTo( 5 );
		p.next( step );
		CHECK( step == 6 );
		p.jumpTo( 10000 );
		p.next( step );
		CHECK( step == 8 );
	}
}

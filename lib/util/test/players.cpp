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

TEST_CASE( "Util::Players", "[players][forwardloop]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	SECTION( "a 1 sized player" )
	{
		Player p( Players::ForwardLoop( 1 ) );

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
		Player p( Players::ForwardLoop( 3 ) );

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
		CHECK( step == 0 );
	}

	SECTION( "increment N" )
	{
		Player p( Players::ForwardLoop( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 1 );
		p.next( step );
		CHECK( step == 1 );
		p.next( 1, step );
		CHECK( step == 1 );
		p.next( 2, step );
		CHECK( step == 2 );
		p.increment( 1 );
		p.next( step );
		CHECK( step == 2 );
		p.next( 3, step );
		CHECK( step == 4 );
		p.increment( 2 );
		p.next( step );
		CHECK( step == 4 );
		p.increment( 10 );
		p.next( step );
		CHECK( step == 4 );
		p.increment( 11 );
		p.next( step );
		CHECK( step == 5 );
	}

	SECTION( "jumping" )
	{
		Player p( Players::ForwardLoop( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 1 );
		p.jumpTo( 5 );
		p.next( step );
		CHECK( step == 6 );
		p.jumpTo( 10000 );
		p.current( step );
		CHECK( step == 9 );
	}
}

TEST_CASE( "Util::Players", "[players][backward]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	SECTION( "a 1 sized player" )
	{
		Player p( Players::Backward( 1 ) );

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
		Player p( Players::Backward( 3 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );

		p	 = Players::Backward( 3, 2 );
		step = 0;
		p.current( step );
		CHECK( step == 2 );
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.current( step );
		CHECK( step == 0 );
		p.increment();
		p.current( step );
		CHECK( step == 0 );
		p.increment();
		p.current( step );
		CHECK( step == 0 );
	}

	SECTION( "increment N" )
	{
		Player p( Players::Backward( 10, 9 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 8 );
		p.next( step );
		CHECK( step == 8 );
		p.current( step );
		CHECK( step == 9 );
		p.increment( 1 );
		p.current( step );
		CHECK( step == 8 );
		p.increment( 2 );
		p.current( step );
		CHECK( step == 6 );
		p.next( 2, step );
		CHECK( step == 4 );
		p.increment( 10 );
		p.current( step );
		CHECK( step == 0 );
		p.increment( 11 );
		p.current( step );
		CHECK( step == 0 );
	}

	SECTION( "jumping" )
	{
		Player p( Players::Backward( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 0 );
		p.jumpTo( 5 );
		p.next( step );
		CHECK( step == 4 );
		p.jumpTo( 10000 );
		p.current( step );
		CHECK( step == 9 );
		p.next( step );
		CHECK( step == 8 );
	}
}

TEST_CASE( "Util::Players", "[players][backwardloop]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	SECTION( "a 1 sized player" )
	{
		Player p( Players::BackwardLoop( 1 ) );

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
		Player p( Players::BackwardLoop( 3 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 2 );
		p.increment();
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );

		p	 = Players::BackwardLoop( 3, 2 );
		step = 0;
		p.current( step );
		CHECK( step == 2 );
		p.next( step );
		CHECK( step == 1 );
		p.increment();
		p.next( step );
		CHECK( step == 0 );
		p.increment();
		p.current( step );
		CHECK( step == 0 );
		p.increment();
		p.current( step );
		CHECK( step == 2 );
		p.increment();
		p.current( step );
		CHECK( step == 1 );
	}

	SECTION( "increment N" )
	{
		Player p( Players::BackwardLoop( 10, 9 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 8 );
		p.next( step );
		CHECK( step == 8 );
		p.increment( 1 );
		p.next( step );
		CHECK( step == 7 );
		p.increment( 2 );
		p.next( step );
		CHECK( step == 5 );
		p.increment( 10 );
		p.next( step );
		CHECK( step == 5 );
		p.increment( 11 );
		p.next( step );
		CHECK( step == 4 );
		p.increment( 22 );
		p.next( step );
		CHECK( step == 2 );
		p.current( step );
		CHECK( step == 3 );
		p.increment( 25 );
		p.current( step );
		CHECK( step == 8 );
		p.increment( 18 );
		p.current( step );
		CHECK( step == 0 );
		p.increment( 13 );
		p.current( step );
		CHECK( step == 7 );
	}

	SECTION( "jumping" )
	{
		Player p( Players::BackwardLoop( 10 ) );

		uint step = 0;
		p.next( step );
		CHECK( step == 9 );
		p.jumpTo( 5 );
		p.next( step );
		CHECK( step == 4 );
		p.jumpTo( 10000 );
		p.current( step );
		CHECK( step == 9 );
		p.next( step );
		CHECK( step == 8 );
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
			p.next( 11, step );
			CHECK( step == 4 );
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
			p.increment( 0 );
			p.current( step );
			CHECK( step == 3 );
			p.next( 2, step );
			CHECK( step == 5 );
			p.increment( 1 );
			p.current( step );
			CHECK( step == 4 );
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
		p.jumpTo( 2 );
		p.next( step );
		CHECK( step == 1 );
		p.jumpTo( 3 );
		p.next( step );
		CHECK( step == 4 );
	}
}

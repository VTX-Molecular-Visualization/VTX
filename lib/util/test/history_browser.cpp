#include <catch2/catch_test_macros.hpp>
#include <util/history_browser.hpp>

TEST_CASE( "Util::HistoryBrowser", "[util][history]" )
{
	VTX::Util::HistoryBrowser history;
	CHECK( history.empty() == true );
	history.add( "1" );
	CHECK( history.empty() == false );
	history.add( "2" );
	history.add( "3" );
	history.tryMoveBackward();
	CHECK( history.currentString() == "3" );
	history.tryMoveBackward();
	CHECK( history.currentString() == "2" );
	history.tryMoveBackward();
	CHECK( history.currentString() == "1" );
	history.tryMoveBackward();
	CHECK( history.currentString() == "1" );
	history.tryMoveForward();
	CHECK( history.currentString() == "2" );
	history.tryMoveForward();
	CHECK( history.currentString() == "3" );
	history.tryMoveForward();
	CHECK( history.currentString() == "3" );

	history.tryMoveBackward();
	history.tryMoveBackward();
	history.resetBrowsing();
	history.tryMoveBackward();
	CHECK( history.currentString() == "3" );

	CHECK( history.last() == "3" );

	history.resetBrowsing();
	history.tryMoveForward();
	CHECK( history.currentString().empty() );
}

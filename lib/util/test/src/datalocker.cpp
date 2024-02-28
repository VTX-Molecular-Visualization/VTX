#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <util/datalocker.hpp>

namespace
{

	struct DummyData
	{
		bool computationDone = false;
	};

	void delayedTurnOn( VTX::Util::ReservedData<DummyData> p_data ) noexcept
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		p_data->computationDone = true;
	}
} // namespace
TEST_CASE( "Util::DataLocker - isDataAvailable", "[datalocker]" )
{
	VTX::Util::DataLocker<DummyData> lockedData;

	CHECK( lockedData.isDataAvailable() );

	{
		auto unlockedData = lockedData.open();

		CHECK( lockedData.isDataAvailable() == false );
	}

	CHECK( lockedData.isDataAvailable() );
}
TEST_CASE( "Util::DataLocker - open locking mechanism", "[datalocker][slow]" )
{
	VTX::Util::DataLocker<DummyData> lockedData;

	std::atomic_bool jobStarted = false;
	std::thread	 delayed_job { [ & ]( )
							   {
								   auto d						  = lockedData.open();
								   jobStarted = true;
								   delayedTurnOn( std::move( d ) );
							   } };
	while ( jobStarted == false )
		;
	auto unlockedData = lockedData.open(); // should block until the thread is over

	CHECK( unlockedData->computationDone );
	
	delayed_job.join(); // thanks for nothing macos
}
/*
// Only way I found to make it crash
TEST_CASE( "Util::DataLocker - crash test", "[datalocker][crash]" )
{
	std::optional<VTX::Util::DataLocker<DummyData>> lockedData;
	lockedData.emplace();

	auto data = lockedData->open();

	lockedData.reset();

	data->computationDone = true;
}
*/

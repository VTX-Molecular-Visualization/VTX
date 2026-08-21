#include <app/action/trajectory.hpp>
#include <app/extractor/system.hpp>
#include <app/fixture.hpp>
#include <app/helper/trajectory.hpp>
#include <app/pass/pass_manager.hpp>
#include <app/services.hpp>
#include <app/trajectory/loader.hpp>
#include <app/trajectory/player.hpp>
#include <app/trajectory/types.hpp>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>
#include <util/ecs.hpp>
#include <util/filesystem.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>

using namespace VTX;

namespace
{
	struct TrajectoryPlayerLifecycleObserver
	{
		TrajectoryPlayerLifecycleObserver()
		{
			App::REG().on_construct<App::Trajectory::Player>().connect<&TrajectoryPlayerLifecycleObserver::onConstruct>(
				this
			);
			App::REG().on_destroy<App::Trajectory::Player>().connect<&TrajectoryPlayerLifecycleObserver::onDestroy>(
				this
			);
		}

		~TrajectoryPlayerLifecycleObserver()
		{
			App::REG()
				.on_construct<App::Trajectory::Player>()
				.disconnect<&TrajectoryPlayerLifecycleObserver::onConstruct>( this );
			App::REG().on_destroy<App::Trajectory::Player>().disconnect<&TrajectoryPlayerLifecycleObserver::onDestroy>(
				this
			);
		}

		void onConstruct( Registry &, const Entity ) { constructCount++; }

		void onDestroy( Registry &, const Entity ) { destroyCount++; }

		uint constructCount = 0;
		uint destroyCount	= 0;
	};

	bool waitUntil( const std::function<bool()> & p_predicate )
	{
		const auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds( 5 );
		while ( not p_predicate() && std::chrono::steady_clock::now() < timeout )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
		}
		return p_predicate();
	}

	void associateTrajectory( const Entity p_entity, const FilePath & p_path )
	{
		auto					   extractor		= std::make_shared<App::Extractor::System>( p_entity, p_path );
		Util::Thread::BaseThread & extractionThread = App::THREAD().createThread(
			[ extractor ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
			{ return ( *extractor )( std::move( p_stopToken ), std::ref( p_thread ) ); }
		);
		extractionThread.wait();
		App::PASS().update( 0.f, 0.f );
		extractor->wait();
	}
} // namespace

TEST_CASE( "VTX_APP - Trajectory loader storage size", "[unit][trajectory]" )
{
	using namespace VTX::App;

	Extractor::TrajectoryBufferSettings settings;
	settings.circularBufferFrameCount = 32;
	CHECK( settings.getStorageFrameCount( Trajectory::TRAJECTORY_BUFFER_MODE::FULL, 250 ) == 250 );
	CHECK( settings.getStorageFrameCount( Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR, 250 ) == 32 );
	CHECK( settings.getStorageFrameCount( Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR, 12 ) == 12 );
	settings.circularBufferFrameCount = 0;
	CHECK( settings.getStorageFrameCount( Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR, 12 ) == 1 );
}

TEST_CASE( "VTX_APP - Trajectory buffer mode selection", "[unit][trajectory]" )
{
	using namespace VTX::App;

	constexpr size_t					atomCount	   = 10;
	constexpr size_t					frameCount	   = 4;
	constexpr size_t					fullBufferSize = atomCount * sizeof( VTX::Vec3f ) * frameCount;
	Extractor::TrajectoryBufferSettings settings;
	settings.maxFullBufferByteSize = fullBufferSize;

	CHECK( settings.selectMode( atomCount, frameCount ) == Trajectory::TRAJECTORY_BUFFER_MODE::FULL );
	settings.maxFullBufferByteSize = fullBufferSize - 1;
	CHECK( settings.selectMode( atomCount, frameCount ) == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR );
	settings.maxFullBufferByteSize = fullBufferSize;
	CHECK( settings.selectMode( atomCount, frameCount + 1 ) == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR );
	settings.maxFullBufferByteSize = 0;
	CHECK( settings.selectMode( 0, frameCount ) == Trajectory::TRAJECTORY_BUFFER_MODE::FULL );
}

TEST_CASE( "VTX_APP - Circular trajectory frame windows", "[unit][trajectory]" )
{
	using namespace VTX::App::Trajectory;

	CHECK(
		VTX::App::Helper::Trajectory::getFrameWindow( 4, 10, 3, TRAJECTORY_READ_DIRECTION::FORWARD )
		== FrameRange( 4, 7 )
	);
	CHECK(
		VTX::App::Helper::Trajectory::getFrameWindow( 9, 10, 3, TRAJECTORY_READ_DIRECTION::FORWARD )
		== FrameRange( 7, 10 )
	);
	CHECK(
		VTX::App::Helper::Trajectory::getFrameWindow( 5, 10, 3, TRAJECTORY_READ_DIRECTION::BACKWARD )
		== FrameRange( 3, 6 )
	);
	CHECK(
		VTX::App::Helper::Trajectory::getFrameWindow( 0, 10, 3, TRAJECTORY_READ_DIRECTION::BACKWARD )
		== FrameRange( 0, 3 )
	);
}

TEST_CASE( "VTX_APP - Trajectory loader circular mapping", "[unit][trajectory]" )
{
	using namespace VTX::App::Trajectory;

	uint	   firstFrameAvailable	= 0;
	uint	   loadedFrameCount		= 1;
	const auto getStorageFrameIndex = [ & ]( const uint p_frame, const size_t p_storageCount )
	{
		return resolveStorageFrameIndex(
			p_frame, p_storageCount, TRAJECTORY_BUFFER_MODE::CIRCULAR, firstFrameAvailable, loadedFrameCount
		);
	};

	SECTION( "Partially filled" )
	{
		loadedFrameCount = 4;

		CHECK(
			FrameRange::fromFirstCount( firstFrameAvailable, loadedFrameCount ) == FrameRange::fromFirstCount( 0, 4 )
		);
		for ( uint frame = 0; frame < 4; frame++ )
		{
			CHECK( getStorageFrameIndex( frame, 5 ) == frame );
		}
		CHECK_FALSE( getStorageFrameIndex( 4, 5 ).has_value() );
	}

	SECTION( "Exactly full" )
	{
		loadedFrameCount = 5;

		CHECK(
			FrameRange::fromFirstCount( firstFrameAvailable, loadedFrameCount ) == FrameRange::fromFirstCount( 0, 5 )
		);
		for ( uint frame = 0; frame < 5; frame++ )
		{
			CHECK( getStorageFrameIndex( frame, 5 ) == frame );
		}
	}

	SECTION( "First wrap" )
	{
		std::array<uint, 5> storage;
		for ( uint frame = 0; frame <= 5; frame++ )
		{
			storage[ frame % storage.size() ] = frame;
		}

		firstFrameAvailable = 1;
		loadedFrameCount	= 5;

		CHECK(
			FrameRange::fromFirstCount( firstFrameAvailable, loadedFrameCount ) == FrameRange::fromFirstCount( 1, 5 )
		);
		for ( uint frame = 1; frame <= 5; frame++ )
		{
			const std::optional<size_t> storageIndex = getStorageFrameIndex( frame, storage.size() );
			REQUIRE( storageIndex );
			CHECK( storage[ *storageIndex ] == frame );
		}
		CHECK_FALSE( getStorageFrameIndex( 0, storage.size() ).has_value() );
		CHECK_FALSE( getStorageFrameIndex( 6, storage.size() ).has_value() );
	}

	SECTION( "Multiple wraps" )
	{
		firstFrameAvailable = 5;
		loadedFrameCount	= 3;

		CHECK(
			FrameRange::fromFirstCount( firstFrameAvailable, loadedFrameCount ) == FrameRange::fromFirstCount( 5, 3 )
		);
		CHECK( getStorageFrameIndex( 5, 3 ) == 2 );
		CHECK( getStorageFrameIndex( 6, 3 ) == 0 );
		CHECK( getStorageFrameIndex( 7, 3 ) == 1 );
		CHECK_FALSE( getStorageFrameIndex( 4, 3 ).has_value() );
		CHECK_FALSE( getStorageFrameIndex( 8, 3 ).has_value() );
	}
}

TEST_CASE( "VTX_APP - Trajectory loader full mapping", "[unit][trajectory]" )
{
	using namespace VTX::App::Trajectory;

	CHECK( resolveStorageFrameIndex( 0, 10, TRAJECTORY_BUFFER_MODE::FULL, 0, 3 ) == 0 );
	CHECK( resolveStorageFrameIndex( 2, 10, TRAJECTORY_BUFFER_MODE::FULL, 0, 3 ) == 2 );
	CHECK_FALSE( resolveStorageFrameIndex( 3, 10, TRAJECTORY_BUFFER_MODE::FULL, 0, 3 ).has_value() );
}

TEST_CASE( "VTX_APP - Loaded trajectory batch validity", "[unit][trajectory]" )
{
	using namespace VTX::App::Trajectory;

	LoadedFrameBatch batch;
	batch.availableFrames = FrameRange( 7, 10 );

	CHECK( isLoadedFrameBatchCurrent( batch, TRAJECTORY_BUFFER_MODE::FULL, 0 ) );
	CHECK( isLoadedFrameBatchCurrent( batch, TRAJECTORY_BUFFER_MODE::CIRCULAR, 8 ) );
	CHECK_FALSE( isLoadedFrameBatchCurrent( batch, TRAJECTORY_BUFFER_MODE::CIRCULAR, 0 ) );
}

TEST_CASE( "VTX_APP - Trajectory player replacement lifecycle", "[integration][trajectory]" )
{
	using namespace VTX::App;

	Fixture app;
	app.loadSystem( std::string_view( "1gcn.pdb" ) );

	auto systems = REG().view<Core::Struct::Topology>();
	REQUIRE( systems.begin() != systems.end() );
	const Entity entity = *systems.begin();

	TrajectoryPlayerLifecycleObserver observer;
	const FilePath					  dataDirectory = Util::Filesystem::getExecutableDir() / "data";

	associateTrajectory( entity, dataDirectory / "1gcn_traj.xtc" );
	CHECK( observer.constructCount == 1 );
	CHECK( observer.destroyCount == 0 );
	CHECK( REG().all_of<Trajectory::Player, Trajectory::Loader>( entity ) );

	associateTrajectory( entity, dataDirectory / "1gcn_traj.xtc" );
	CHECK( observer.constructCount == 2 );
	CHECK( observer.destroyCount == 1 );
	CHECK( REG().all_of<Trajectory::Player, Trajectory::Loader>( entity ) );

	associateTrajectory( entity, dataDirectory / "1gcn.pdb" );
	CHECK( observer.constructCount == 2 );
	CHECK( observer.destroyCount == 2 );
	CHECK_FALSE( REG().any_of<Trajectory::Player, Trajectory::Loader>( entity ) );
}

TEST_CASE( "VTX_APP - Circular trajectory loading", "[integration][trajectory]" )
{
	using namespace VTX;
	using namespace VTX::App;

	constexpr size_t circularCapacity = 3;

	Fixture app;
	app.loadSystem( std::string_view( "1gcn.pdb" ) );

	auto systems = REG().view<Core::Struct::Topology>();
	REQUIRE( systems.begin() != systems.end() );
	const Entity entity = *systems.begin();

	const FilePath trajectoryPath = Util::Filesystem::getExecutableDir() / "data" / "1gcn_traj.xtc";
	Extractor::TrajectoryBufferSettings bufferSettings;
	bufferSettings.maxFullBufferByteSize	= 0;
	bufferSettings.circularBufferFrameCount = circularCapacity;
	auto extractor
		= std::make_shared<Extractor::System>( entity, trajectoryPath, IO::READER_OPTION::ALL, bufferSettings );

	Util::Thread::BaseThread & extractionThread = THREAD().createThread(
		[ extractor ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
		{ return ( *extractor )( std::move( p_stopToken ), std::ref( p_thread ) ); }
	);
	extractionThread.wait();
	PASS().update( 0.f, 0.f );
	extractor->wait();

	const auto & loader = REG().get<Trajectory::Loader>( entity );
	CHECK( loader.mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR );

	const auto & trajectory = REG().get<Core::Struct::Trajectory>( entity );
	REQUIRE( trajectory.frameCount > circularCapacity );
	CHECK( trajectory.frames.size() == circularCapacity );

	const Core::Struct::Topology & topology = REG().get<Core::Struct::Topology>( entity );
	Action::Trajectory::SetPaused().execute( entity, true );

	const uint lastFrame = static_cast<uint>( trajectory.frameCount - 1 );
	Action::Trajectory::JumpTo().execute( entity, lastFrame );
	PASS().update( 0.f, 0.f );
	const uint64_t staleRequestVersion = loader.thread->getRequestVersion();
	Action::Trajectory::JumpTo().execute( entity, 0 );
	PASS().update( 0.f, 0.f );
	CHECK( loader.thread->getRequestVersion() > staleRequestVersion );
	REQUIRE( waitUntil(
		[ entity ]
		{
			PASS().update( 0.f, 0.f );
			return Helper::Trajectory::isFrameAvailable( entity, 0 );
		}
	) );

	Action::Trajectory::JumpTo().execute( entity, lastFrame );
	PASS().update( 0.f, 0.f );
	REQUIRE( waitUntil(
		[ entity, lastFrame ]
		{
			PASS().update( 0.f, 0.f );
			return Helper::Trajectory::isFrameAvailable( entity, lastFrame );
		}
	) );
	PASS().update( 0.f, 0.f );

	Trajectory::FrameRange availableFrames = loader.availableFrames;
	CHECK( availableFrames == Trajectory::FrameRange( lastFrame + 1 - circularCapacity, lastFrame + 1 ) );
	CHECK(
		Helper::Trajectory::visitFrame(
			entity,
			lastFrame,
			[ &topology ]( const Core::Struct::FrameView p_frame )
			{ CHECK( p_frame.size() == topology.getAtomCount() ); }
		)
	);
	CHECK( REG().get<Trajectory::Player>( entity ).currentFrameIndex == lastFrame );

	Action::Trajectory::JumpTo().execute( entity, 0 );
	PASS().update( 0.f, 0.f );
	REQUIRE( waitUntil(
		[ entity ]
		{
			PASS().update( 0.f, 0.f );
			return Helper::Trajectory::isFrameAvailable( entity, 0 );
		}
	) );
	PASS().update( 0.f, 0.f );

	availableFrames = loader.availableFrames;
	CHECK( availableFrames == Trajectory::FrameRange( 0, circularCapacity ) );
	CHECK(
		Helper::Trajectory::visitFrame(
			entity,
			0,
			[ &topology ]( const Core::Struct::FrameView p_frame )
			{ CHECK( p_frame.size() == topology.getAtomCount() ); }
		)
	);
	CHECK( REG().get<Trajectory::Player>( entity ).currentFrameIndex == 0 );

	const uint64_t requestVersion = loader.thread->getRequestVersion();
	PASS().update( 1000.f, 0.f );
	std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	CHECK( loader.thread->getRequestVersion() == requestVersion );
	CHECK( loader.availableFrames == availableFrames );

	Action::Trajectory::JumpTo().execute( entity, lastFrame );
	PASS().update( 0.f, 0.f );
	REG().destroy( entity );
	CHECK_FALSE( REG().valid( entity ) );
}

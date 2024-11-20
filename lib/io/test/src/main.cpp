#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/network.hpp>
#include <core/struct/frame_data.hpp>
#include <fstream>

TEST_CASE( "VTX_IO - Test filepath", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	   = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	   = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	VTX_INFO( "Test on {}", systemName );

	VTX::Core::Struct::System system		 = VTX::Core::Struct::System();
	IO::Reader::System		  systemReader = IO::Reader::System();

	systemReader.readFile( systemPath, system );

	CHECK( system.getChainCount() == 62 );
	CHECK( system.getResidueCount() == 11381 );
	CHECK( system.getAtomCount() == 113095 );
	CHECK( system.getBondCount() == 129957 );
}

TEST_CASE( "VTX_IO - Test buffer", "[integration]" )
{
	return;

	using namespace VTX;
	using namespace VTX::IO;

	const std::string url = "https://files.rcsb.org/download/4hhb.pdb";

	std::string data;
	Util::Network::httpRequestGet( url, &data );

	VTX_INFO( "Test on {}", url );

	VTX::Core::Struct::System system		 = VTX::Core::Struct::System();
	IO::Reader::System		  systemReader = IO::Reader::System();

	systemReader.readBuffer( data, "4hhb.mmtf", system );

	CHECK( system.getChainCount() == 14 );
	CHECK( system.getResidueCount() == 801 );
	CHECK( system.getAtomCount() == 4779 );
	CHECK( system.getBondCount() == 129960 );
}

TEST_CASE( "VTX_IO - Benchmark", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	   = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	   = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	VTX_INFO( "Benchmark on {}.", systemName );

	BENCHMARK( "Open systems" )
	{
		VTX::Core::Struct::System system		 = VTX::Core::Struct::System();
		IO::Reader::System		  systemReader = IO::Reader::System();

		systemReader.readFile( systemPath, system );
	};
}

/// devjla

TEST_CASE( "VTX_IO - dummy write", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "2ama_1_npt";
	const std::string moleculePathname = moleculeName + ".trr";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/trajs" / moleculePathname;
	const std::string dummyName		   = "dummy";
	const std::string dummyPathname	   = dummyName + ".txt";
	std::ofstream	  outFile( dummyPathname );

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule	   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader	   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	/* for ( auto & element : molecule.trajectory.frames[ 0 ] )
	{
		outFile << element[0] << element[1] << element[2];
	} */
	for ( auto & element : molecule.trajectory.GetCurrentFrame() )
	{
		outFile << element[ 0 ] << element[ 1 ] << element[ 2 ];
	}
	outFile.close();
	// devjla
	// CHECK( molecule.trajectory.frames.size() == 10001 );
	CHECK( molecule.trajectory.GetFrameCount() == 10001 );
}

TEST_CASE( "VTX_IO - Test filepath trajectory 2am9", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "2am9_md_0_1";
	const std::string moleculePathname = moleculeName + ".xtc";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/trajs" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 10001 );
	CHECK( molecule.trajectory.GetFrameCount() == 10001 );
}

TEST_CASE( "VTX_IO - Test filepath trajectory 2ama", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "2ama_1_npt";
	const std::string moleculePathname = moleculeName + ".trr";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/trajs" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 101 );
	CHECK( molecule.trajectory.GetFrameCount() == 101 );
}

TEST_CASE( "VTX_IO - Test filepath trajectory 2pip", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "2pip_1_npt";
	const std::string moleculePathname = moleculeName + ".trr";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/trajs" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 101 );
	CHECK( molecule.trajectory.GetFrameCount() == 101 );
}

TEST_CASE( "VTX_IO - Test filepath trajectory 5vo4", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "5vo4_1_npt";
	const std::string moleculePathname = moleculeName + ".trr";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/trajs" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 101 );
	CHECK( molecule.trajectory.GetFrameCount() == 101 );
}

TEST_CASE( "VTX_IO - debug cif client", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "machinery";
	const std::string moleculePathname = moleculeName + ".cif";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/debug_cif" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 1 );
	CHECK( molecule.trajectory.GetFrameCount() == 1 );
}

TEST_CASE( "VTX_IO - debug cif multiline", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "1cpc";
	const std::string moleculePathname = moleculeName + ".cif";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data/debug_cif" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		   = VTX::Core::Struct::System();
	molecule.trajectory.SetOptimized();
	IO::Reader::System moleculeReader		   = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	// devjla
	// CHECK( molecule.trajectory.frames.size() == 1 );
	CHECK( molecule.trajectory.GetFrameCount() == 1 );
}

TEST_CASE( "VTX_IO - circular buffer read write no overflow", "[integration]" )
{
	std::vector<int>					   bigVect { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	VTX::Core::Struct::CircularBuffer<int> testCircBuff( 5 );
	for ( int idx = 0; idx < 4; ++idx )
		testCircBuff.WriteElement( bigVect[ idx ] );

	int val( INT_MAX );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 0 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 1 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 2 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 3 );
	CHECK( testCircBuff.ReadElement( val ) == false );
	CHECK( val == 3 );
}

TEST_CASE( "VTX_IO - circular buffer write full buffer", "[integration]" )
{
	std::vector<int>					   bigVect { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	VTX::Core::Struct::CircularBuffer<int> testCircBuff( 5 );
	for ( int idx = 0; idx < 5; ++idx )
		testCircBuff.WriteElement( bigVect[ idx ] );

	int val( INT_MAX );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 0 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 1 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 2 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 3 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 4 );
}

TEST_CASE( "VTX_IO - circular buffer write overflow circular read", "[integration]" )
{
	std::vector<int>					   bigVect { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	VTX::Core::Struct::CircularBuffer<int> testCircBuff( 5 );
	for ( int idx = 0; idx < 5; ++idx )
		testCircBuff.WriteElement( bigVect[ idx ] );

	int val( INT_MAX );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 0 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 1 );

	testCircBuff.WriteElement( bigVect[ 5 ] );

	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 2 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 3 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 4 );
	CHECK( testCircBuff.ReadElement( val ) == true );
	CHECK( val == 5 );
	CHECK( testCircBuff.ReadElement( val ) == false );
	CHECK( val == 5 );
}

TEST_CASE( "VTX_IO - frame data write", "[integration]" )
{
	VTX::Core::Struct::FrameData frames;
	for ( uint64_t idx = 0; idx < 5; ++idx )
	{
		const std::vector<VTX::Vec3f> wvec { { idx, idx, idx } };
		std::vector<VTX::Vec3f> &	  dummyFrame = frames.WriteElement( wvec );
		++dummyFrame[ 0 ][ 2 ];
	}

	std::vector<VTX::Vec3f> rvec { { INT_MAX, INT_MAX, INT_MAX } };
	CHECK( frames.ReadElement( rvec ) == true );
	CHECK( rvec[ 0 ][ 0 ] == 0 );
	CHECK( rvec[ 0 ][ 2 ] == 1 );
	CHECK( frames.ReadElement( rvec ) == true );
	CHECK( rvec[ 0 ][ 0 ] == 1 );
	CHECK( rvec[ 0 ][ 2 ] == 2 );
	CHECK( frames.ReadElement( rvec ) == true );
	CHECK( rvec[ 0 ][ 0 ] == 2 );
	CHECK( rvec[ 0 ][ 2 ] == 3 );
	CHECK( frames.ReadElement( rvec ) == true );
	CHECK( rvec[ 0 ][ 0 ] == 3 );
	CHECK( rvec[ 0 ][ 2 ] == 4 );
	CHECK( frames.ReadElement( rvec ) == true );
	CHECK( rvec[ 0 ][ 0 ] == 4 );
	CHECK( rvec[ 0 ][ 2 ] == 5 );
}

TEST_CASE( "VTX_IO - model frame write read", "[integration]" )
{
	VTX::Core::Struct::FrameData  frames;
	const uint64_t				  modelVal( 42 );
	const std::vector<VTX::Vec3f> wModel { { modelVal, modelVal, modelVal } };
	std::vector<VTX::Vec3f> &	  blankModel = frames.GetModelFrame();
	blankModel								 = wModel;
	std::vector<VTX::Vec3f> testFrame { { INT_MAX, INT_MAX, INT_MAX } };

	testFrame = frames.GetModelFrame();
	CHECK( testFrame[ 0 ][ 0 ] == modelVal );
	CHECK( testFrame[ 0 ][ 1 ] == modelVal );
	CHECK( testFrame[ 0 ][ 2 ] == modelVal );
}

TEST_CASE( "VTX_IO - frame write full buffer", "[integration]" )
{
	VTX::Core::Struct::FrameData frames;
	for ( uint64_t idx = 0; idx < 49; ++idx )
	{
		const std::vector<VTX::Vec3f> wvec { { idx, idx, idx } };
		std::vector<VTX::Vec3f> &	  dummyFrame = frames.WriteElement( wvec );
		++dummyFrame[ 0 ][ 2 ];
	}

	std::vector<VTX::Vec3f> vals { { INT_MAX, INT_MAX, INT_MAX } };
	for ( uint64_t idx = 0; idx < 49; ++idx )
	{
		CHECK( frames.ReadElement( vals ) == true );
		CHECK( vals[ 0 ][ 0 ] == idx );
	}
}

TEST_CASE( "VTX_IO - frame write overflow circular read", "[integration]" )
{
	VTX::Core::Struct::FrameData frames;
	for ( uint64_t idx = 0; idx < 49; ++idx )
	{
		const std::vector<VTX::Vec3f> wvec { { idx, idx, idx } };
		std::vector<VTX::Vec3f> &	  dummyFrame = frames.WriteElement( wvec );
		++dummyFrame[ 0 ][ 2 ];
	}

	std::vector<VTX::Vec3f> vals { { INT_MAX, INT_MAX, INT_MAX } };
	for ( uint64_t idx = 0; idx < 9; ++idx )
	{
		CHECK( frames.ReadElement( vals ) == true );
		CHECK( vals[ 0 ][ 0 ] == idx );
	}

	frames.WriteElement( { { 42, 42, 42 } } );

	for ( uint64_t idx = 0; idx < 40; ++idx )
	{
		CHECK( frames.ReadElement( vals ) == true );
		CHECK( vals[ 0 ][ 0 ] == idx + 9 );
	}
	CHECK( frames.ReadElement( vals ) == true );
	CHECK( vals[ 0 ][ 0 ] == 42 );
	CHECK( frames.ReadElement( vals ) == false );
	CHECK( vals[ 0 ][ 0 ] == 42 );
}

TEST_CASE( "VTX_IO - frame write overflow circular read and model frame", "[integration]" )
{
	VTX::Core::Struct::FrameData frames;

	const uint64_t				  modelVal( 142 );
	const std::vector<VTX::Vec3f> wModel { { modelVal, modelVal + 1, modelVal + 2 } };
	std::vector<VTX::Vec3f> &	  blankModel = frames.GetModelFrame();
	blankModel								 = wModel;
	std::vector<VTX::Vec3f> testFrame { { INT_MAX, INT_MAX, INT_MAX } };

	for ( uint64_t idx = 0; idx < 49; ++idx )
	{
		const std::vector<VTX::Vec3f> wvec { { idx, idx, idx } };
		std::vector<VTX::Vec3f> &	  dummyFrame = frames.WriteElement( wvec );
		++dummyFrame[ 0 ][ 2 ];
	}

	std::vector<VTX::Vec3f> vals { { INT_MAX, INT_MAX, INT_MAX } };
	for ( uint64_t idx = 0; idx < 9; ++idx )
	{
		CHECK( frames.ReadElement( vals ) == true );
		CHECK( vals[ 0 ][ 0 ] == idx );
	}

	frames.WriteElement( { { 42, 42, 42 } } );

	for ( uint64_t idx = 0; idx < 40; ++idx )
	{
		CHECK( frames.ReadElement( vals ) == true );
		CHECK( vals[ 0 ][ 0 ] == idx + 9 );
	}
	CHECK( frames.ReadElement( vals ) == true );
	CHECK( vals[ 0 ][ 0 ] == 42 );
	CHECK( frames.ReadElement( vals ) == false );
	CHECK( vals[ 0 ][ 0 ] == 42 );

	testFrame = frames.GetModelFrame();
	CHECK( testFrame[ 0 ][ 0 ] == modelVal );
	CHECK( testFrame[ 0 ][ 1 ] == modelVal + 1 );
	CHECK( testFrame[ 0 ][ 2 ] == modelVal + 2 );
}
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <core/struct/system.hpp>
#include <util/filesystem.hpp>
#include <util/thread.hpp>
//
#include <io/reader.hpp>

TEST_CASE( "VTX_IO - Test filepath", "[reader][topology]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	 = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	VTX::Core::Struct::Topology system = VTX::Core::Struct::Topology();
	Util::StopToken			  t;
	IO::SystemReader		  systemReader( systemPath, t );

	systemReader.get( system );

	CHECK( system.getChainCount() == 62 );
	CHECK( system.getResidueCount() == 11381 );
	CHECK( system.getAtomCount() == 113095 );
	CHECK( system.getBondCount() == 129957 );
}

TEST_CASE( "VTX_IO - Test filepath", "[reader][positions]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	 = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	std::vector<VTX::Vec3f> pos;
	Util::StopToken			t;
	IO::SystemReader		systemReader( systemPath, t );

	systemReader.get( pos );

	CHECK( pos.size() == 113095 );
}

TEST_CASE( "VTX_IO - Test filepath", "[reader][pdb_code]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	 = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	Util::StopToken	 t;
	IO::SystemReader systemReader( systemPath, t );

	std::string s;
	systemReader.get( VTX::IO::PdbIdCode { &s } );
	CHECK( s == "8OIT" );

	// std::string		 s;
	// VTX::IO::PdbIdCode d { s };
	// systemReader.get( d );
	// CHECK( d.code == "8OIT" );
}

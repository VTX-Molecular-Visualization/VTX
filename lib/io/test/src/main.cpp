#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/network.hpp>

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

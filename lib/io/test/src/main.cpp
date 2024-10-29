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

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data" / moleculePathname;

	VTX_INFO( "Test on {}", moleculeName );

	VTX::Core::Struct::System molecule		 = VTX::Core::Struct::System();
	IO::Reader::System		  moleculeReader = IO::Reader::System();

	moleculeReader.readFile( moleculePath, molecule );

	CHECK( molecule.getChainCount() == 62 );
	CHECK( molecule.getResidueCount() == 11381 );
	CHECK( molecule.getAtomCount() == 113095 );
	CHECK( molecule.getBondCount() == 129957 );
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

	VTX::Core::Struct::System molecule		 = VTX::Core::Struct::System();
	IO::Reader::System		  moleculeReader = IO::Reader::System();

	moleculeReader.readBuffer( data, "4hhb.mmtf", molecule );

	CHECK( molecule.getChainCount() == 14 );
	CHECK( molecule.getResidueCount() == 801 );
	CHECK( molecule.getAtomCount() == 4779 );
	CHECK( molecule.getBondCount() == 129960 );
}

TEST_CASE( "VTX_IO - Benchmark", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data" / moleculePathname;

	VTX_INFO( "Benchmark on {}.", moleculeName );

	BENCHMARK( "Open molecules" )
	{
		VTX::Core::Struct::System molecule		 = VTX::Core::Struct::System();
		IO::Reader::System		  moleculeReader = IO::Reader::System();

		moleculeReader.readFile( moleculePath, molecule );
	};
}

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/molecule.hpp>
#include <io/reader/molecule.hpp>
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

	VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
	IO::Reader::Molecule		moleculeReader = IO::Reader::Molecule();

	moleculeReader.readFile( moleculePath, molecule );

	REQUIRE( molecule.getChainCount() == 79 );
	REQUIRE( molecule.getResidueCount() == 11381 );
	REQUIRE( molecule.getAtomCount() == 113095 );
	REQUIRE( molecule.getBondCount() == 129959 );
}

TEST_CASE( "VTX_IO - Test buffer", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string url = "https://mmtf.rcsb.org/v1.0/full/8OIT";

	std::string data;
	Util::Network::httpRequestGet( url, data );

	VTX_INFO( "Test on {}", url );

	VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
	IO::Reader::Molecule		moleculeReader = IO::Reader::Molecule();

	moleculeReader.readBuffer( data, "8OIT.mmtf", molecule );

	REQUIRE( molecule.getChainCount() == 79 );
	REQUIRE( molecule.getResidueCount() == 11381 );
	REQUIRE( molecule.getAtomCount() == 113095 );
	REQUIRE( molecule.getBondCount() == 129959 );
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
		VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
		IO::Reader::Molecule		moleculeReader = IO::Reader::Molecule();

		moleculeReader.readFile( moleculePath, molecule );
	};
}

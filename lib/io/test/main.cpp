#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/flat_struct/molecule.hpp>
#include <core/struct/molecule.hpp>
#include <io/reader/flat_molecule.hpp>
#include <io/reader/molecule.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_IO - Test Struct", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = std::filesystem::current_path() / "data" / moleculePathname;

	VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
	IO::Reader::Molecule		moleculeReader = IO::Reader::Molecule();

	moleculeReader.readFile( moleculePath, molecule );

	REQUIRE( molecule.getChainCount() == 79 );
	REQUIRE( molecule.getResidueCount() == 11381 );
	REQUIRE( molecule.getAtomCount() == 113095 );
	REQUIRE( molecule.getBondCount() == 129959 );
}

TEST_CASE( "VTX_IO - Test Flat Struct", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = std::filesystem::current_path() / "data" / moleculePathname;

	VTX::Core::FlatStruct::Molecule molecule	   = VTX::Core::FlatStruct::Molecule();
	IO::Reader::FlatMolecule		moleculeReader = IO::Reader::FlatMolecule();

	moleculeReader.readFile( moleculePath, molecule );

	REQUIRE( molecule.getChainCount() == 79 );
	REQUIRE( molecule.getResidueCount() == 11381 );
	REQUIRE( molecule.getAtomCount() == 113095 );
	REQUIRE( molecule.getBondCount() == 129959 );
}

TEST_CASE( "VTX_IO - Benchmark Struct", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = std::filesystem::current_path() / "data" / moleculePathname;

	BENCHMARK( "Open molecules" )
	{
		VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
		IO::Reader::Molecule		moleculeReader = IO::Reader::Molecule();

		moleculeReader.readFile( moleculePath, molecule );
	};
}

TEST_CASE( "VTX_IO - Benchmark Flat Struct", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = std::filesystem::current_path() / "data" / moleculePathname;

	BENCHMARK( "Open molecules" )
	{
		VTX::Core::FlatStruct::Molecule molecule	   = VTX::Core::FlatStruct::Molecule();
		IO::Reader::FlatMolecule		moleculeReader = IO::Reader::FlatMolecule();

		moleculeReader.readFile( moleculePath, molecule );
	};
}

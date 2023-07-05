#include "fill_molecule.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/molecule.hpp>
#include <io/reader/chemfiles.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_IO - Test", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";
	const FilePath	  moleculePath	   = std::filesystem::current_path() / "data" / moleculePathname;

	// BENCHMARK( "Open molecules" )
	//{
	std::unique_ptr<Reader::Chemfiles> chemfileReader = Reader::Chemfiles::readFile( moleculePath );
	Core::Struct::Molecule			   molecule		  = Core::Struct::Molecule();

	Test::fillStructure( *chemfileReader, molecule );
	//};
}

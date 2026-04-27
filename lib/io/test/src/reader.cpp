#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <core/struct/topology.hpp>
#include <util/filesystem.hpp>
#include <util/thread.hpp>
//
#include <io/reader.hpp>

TEST_CASE( "VTX_IO - Test filepath", "[reader][topology][metadata]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	 = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	VTX::Core::Struct::Topology				topology;
	VTX::IO::Metadata						metadata;
	Util::StopToken							t;
	IO::SystemReader						systemReader( systemPath, IO::READER_OPTION::ALL, t );
	VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();

	systemReader.get( dict, topology, metadata );

	CHECK( topology.getChainCount() == 62 );
	CHECK( topology.getResidueCount() == 11381 );
	CHECK( topology.getAtomCount() == 113095 );
	CHECK( topology.getBondCount() == 129957 );
	CHECK( metadata.path == systemPath );
	CHECK( metadata.pdbIDCode == "8OIT" );
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
	IO::SystemReader		systemReader( systemPath, IO::READER_OPTION::ALL, t );

	systemReader.get( pos );

	CHECK( pos.size() == 113095 );
}

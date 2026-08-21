#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <io/reader.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/network.hpp>
#include <util/thread/stop_token.hpp>

TEST_CASE( "VTX_IO - Benchmark", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::IO;

	const std::string systemName	 = "8OIT";
	const std::string systemPathname = systemName + ".mmtf";
	const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

	VTX_INFO( "Benchmark on {}.", systemName );

	BENCHMARK( "Open systems" )
	{
		VTX::Core::Struct::Topology				topology;
		IO::Metadata							metadata;
		std::vector<VTX::Vec3f>					pos;
		Util::Thread::StopToken					t;
		IO::SystemReader						systemReader( systemPath, IO::READER_OPTION::ALL, t );
		VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();

		systemReader.get( dict, topology, metadata );
		systemReader.get( pos );
	};
}

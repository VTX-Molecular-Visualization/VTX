#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/action/visibility.hpp>
#include <app/fixture.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>

TEST_CASE( "VTX_APP - System - Load", "[arguments]" )
{
	using namespace VTX;
	using namespace VTX::App;

	// Preping arguments

	std::string	   execPathStr	 = Util::Filesystem::getExecutableDir().string();
	std::string	   systemPathStr = ( Util::Filesystem::getExecutableDir() / "data" / "1AGA.mmtf" ).string();
	const char *   argv[]		 = { execPathStr.c_str(), systemPathStr.c_str() };
	ArgumentParser parser( 2, argv );
	parser.parse();

	Arguments args;
	parser.get( args );

	args.noGraphics = true;
	args.noUpdates	= true;
	args.noGui		= true;
	args.debug		= true;
	VTXApp app( std::move( args ) );
	app.start();

	//

	uint num_system = 0;
	for ( auto it_topolNTT : App::REG().view<Core::Struct::Topology>() )
	{
		auto & topol = App::REG().get<Core::Struct::Topology>( it_topolNTT );
		num_system++;
		CHECK( topol.getAtomCount() == 126 );
	}
	CHECK( num_system == 1 );
}

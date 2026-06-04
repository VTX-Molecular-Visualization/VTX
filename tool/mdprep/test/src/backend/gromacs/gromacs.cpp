#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
#include <thread>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"
//
#include <tool/mdprep/backends/gromacs/gromacs.hpp>

namespace VTX::test
{
	void setForcefield( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions & in, const char * p_ffName ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		in.pdb2gmx.forcefields = listForcefields(
			VTX::Tool::Mdprep::executableDirectory()
			/ VTX::Tool::Mdprep::backends::Gromacs::defaultFfDirectoryRelativePath()
		);
		auto it = std::find_if(
			in.pdb2gmx.forcefields.begin(),
			in.pdb2gmx.forcefields.end(),
			[ & ]( const forcefield & p_ ) { return p_.getName() == p_ffName; }
		);
		if ( it != std::end( in.pdb2gmx.forcefields ) )
		{
			in.pdb2gmx.forcefieldIndex = std::distance( in.pdb2gmx.forcefields.begin(), it );
		}
	}
} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - prepareStructure 1ubq", "[prepareStructure][1ubq]" )
{
	return; // TMP TODO : Put it back online
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	GromacsInstructions in;

	VTX::test::setForcefield( in, "amber03" );
	in.pdb2gmx.water		   = E_WATER_MODEL::tip3p;
	in.editconf1.bt			   = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.editconf1.d			   = 1.0;
	in.editconf1.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.editconf1.purpose	   = E_EDITCONF_PURPOSE::setup_box;
	in.gromppIons.min.emstep   = 0.009;
	in.gromppIons.min.emtol	   = 1000;
	in.gromppIons.min.nsteps   = 6000;
	in.genion.conc			   = 0.15;
	in.genion.pname			   = "K";
	in.genion.pq			   = 1;
	in.genion.nname			   = "CL";
	in.genion.nq			   = -1;

	in.rootDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareStructure" / "1ubq";
	if ( fs::exists( in.rootDir ) )
	{
		fs::remove_all( in.rootDir );
	}
	fs::create_directories( in.rootDir );

	std::stop_token token;
	prepareStructure( token, VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb", in );
	int i = 0;
	for ( auto & jobData : in.jobData )
	{
		INFO( fmt::format( "Job Number is <{}>", i ) );
		CHECK( jobData.report.errorOccured == false );
		i++;
	}
	for ( auto & outputFileStrPtr : in.outputs.fileStringPtrs )
	{
		CHECK( fs::exists( fs::path( *outputFileStrPtr ) ) );
	}
}

TEST_CASE( "VTX_TOOL_MdPrep - prepareStructure 2wfv", "[prepareStructure][2wfv]" )
{
	return; // TMP TODO : Put it back online
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	GromacsInstructions in;

	VTX::test::setForcefield( in, "amber03" );
	in.pdb2gmx.water		   = E_WATER_MODEL::tip3p;
	in.editconf1.bt			   = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.editconf1.d			   = 1.0;
	in.editconf1.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.editconf1.purpose	   = E_EDITCONF_PURPOSE::setup_box;
	in.gromppIons.min.emstep   = 0.02;
	in.gromppIons.min.emtol	   = 1000;
	in.gromppIons.min.nsteps   = 30;
	in.genion.conc			   = 0.15;
	in.genion.pname			   = "K";
	in.genion.pq			   = 1;
	in.genion.nname			   = "CL";
	in.genion.nq			   = -1;

	in.rootDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareStructure" / "2wfv";
	if ( fs::exists( in.rootDir ) )
	{
		fs::remove_all( in.rootDir );
	}
	fs::create_directories( in.rootDir );

	std::stop_token token;
	prepareStructure( token, VTX::Tool::Mdprep::executableDirectory() / "data" / "2wfv.pdb", in );
	int i = 0;
	for ( auto & jobData : in.jobData )
	{
		INFO( fmt::format( "Job Number is <{}>", i ) );
		CHECK( jobData.report.errorOccured == false );
		i++;
	}
	for ( auto & outputFileStrPtr : in.outputs.fileStringPtrs )
	{
		CHECK( fs::exists( fs::path( *outputFileStrPtr ) ) );
	}
}

TEST_CASE( "VTX_TOOL_MdPrep - SystemTester", "[SystemTester]" )
{
	// Passing ones : 4j6s 2wfv 8hu4.nolig
	// Not Passing ones : 4nxo 8hu4
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	fs::path				dataDir = VTX::Tool::Mdprep::executableDirectory() / "data";
	std::vector<forcefield> ffs
		= listForcefields( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	REQUIRE( ffs.size() > 0 );
	SystemTester s_4j6s( dataDir / "4j6s.pdb", ffs[ 0 ], E_WATER_MODEL::tip3p );
	SystemTester s_2wfv( dataDir / "2wfv.pdb", ffs[ 0 ], E_WATER_MODEL::tip3p );
	SystemTester s_8hu4_nolig( dataDir / "8hu4.nolig.pdb", ffs[ 0 ], E_WATER_MODEL::tip3p );
	SystemTester s_4nxo( dataDir / "4nxo.pdb", ffs[ 0 ], E_WATER_MODEL::tip3p );
	SystemTester s_8hu4( dataDir / "8hu4.pdb", ffs[ 0 ], E_WATER_MODEL::tip3p );
	CHECK( s_2wfv == true );
	CHECK( s_8hu4_nolig == true );
	CHECK( s_4j6s == false );
	CHECK( s_4nxo == false );
	CHECK( s_8hu4 == false );
}

TEST_CASE( "VTX_TOOL_MdPrep - prepPy", "[prepPy]" ) // temporary UT meant to generate runMD.py test env
{
	return; // TMP TODO : Put it back online
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	GromacsInstructions in;

	VTX::test::setForcefield( in, "amber03" );
	in.pdb2gmx.water		   = E_WATER_MODEL::tip3p;
	in.editconf1.bt			   = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.editconf1.d			   = 1.0;
	in.editconf1.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.editconf1.purpose	   = E_EDITCONF_PURPOSE::setup_box;
	in.gromppEm.min.emstep	   = 0.02;
	in.gromppEm.min.emtol	   = 1000;
	in.gromppEm.min.nsteps	   = 100;
	in.genion.conc			   = 0.15;
	in.genion.pname			   = "K";
	in.genion.pq			   = 1;
	in.genion.nname			   = "CL";
	in.genion.nq			   = -1;

	in.rootDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareStructure" / "1gcn";
	if ( fs::exists( in.rootDir ) )
	{
		fs::remove_all( in.rootDir );
	}
	fs::create_directories( in.rootDir );

	std::stop_token token;
	prepareStructure( token, VTX::Tool::Mdprep::executableDirectory() / "data" / "1gcn.pdb", in );
	int i = 0;
	for ( auto & jobData : in.jobData )
	{
		INFO( fmt::format( "Job Number is <{}>", i ) );
		CHECK( jobData.report.errorOccured == false );
		REQUIRE( jobData.report.errorOccured == false );
		i++;
	}
	for ( auto & outputFileStrPtr : in.outputs.fileStringPtrs )
	{
		REQUIRE( fs::exists( fs::path( *outputFileStrPtr ) ) );
	}

	MdInstructions md;
	md.fileStem				   = "1gcn";
	md.nvt.nsteps			   = 100;
	md.nvt.dt				   = 0.002;
	md.nvt.nstxout			   = 100;
	md.nvt.nstvout			   = 0;
	md.nvt.nstenergy		   = 0;
	md.nvt.nstlog			   = 0;
	md.npt.nsteps			   = 100;
	md.npt.dt				   = 0.002;
	md.npt.nstxout			   = 100;
	md.npt.nstvout			   = 0;
	md.npt.nstenergy		   = 0;
	md.npt.nstlog			   = 0;
	md.prod.nsteps			   = 100;
	md.prod.dt				   = 0.002;
	md.prod.nstxout			   = 0;
	md.prod.nstvout			   = 0;
	md.prod.nstenergy		   = 0;
	md.prod.nstlog			   = 5000;
	md.prod.nstxout_compressed = 1;
	fs::path outDir			   = VTX::Tool::Mdprep::executableDirectory() / "out" / "packed" / "1gcn";
	if ( fs::exists( outDir ) )
	{
		fs::remove_all( outDir );
	}
	fs::create_directories( outDir );
	pack( VTX::Tool::Mdprep::executableDirectory() / "out" / "packed" / "1gcn", in.outputs, md );
}

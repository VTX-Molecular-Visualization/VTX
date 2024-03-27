#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
#include <thread>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include "tools/mdprep/gromacs/job.hpp"
#include <tools/mdprep/gromacs/util.hpp>
//
#include <mdprep/test/fixture.hpp>
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>

namespace VTX::test
{
	void setForcefield( VTX::Tool::Mdprep::Gromacs::GromacsInstructions & in, const char * p_ffName ) noexcept
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		in.pdb2gmx.forcefields = listForcefields(
			VTX::Tool::Mdprep::executableDirectory() / VTX::Tool::Mdprep::Gromacs::defaultFfDirectoryRelativePath()
		);
		auto it = std::find_if(
			in.pdb2gmx.forcefields.begin(),
			in.pdb2gmx.forcefields.end(),
			[ & ]( const forcefield & p_ ) { return p_.getName() == p_ffName; }
		);
		if ( it != std::end( in.pdb2gmx.forcefields ) )
			in.pdb2gmx.forcefieldIndex = std::distance( in.pdb2gmx.forcefields.begin(), it );
	}
} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - prepareStructure", "[prepareStructure][1ubq]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	GromacsInstructions in;

	VTX::test::setForcefield( in, "amber03" );
	in.pdb2gmx.water		   = E_WATER_MODEL::tip3p;
	in.editconf1.bt			   = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.editconf1.d			   = 1.0;
	in.editconf1.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.editconf1.purpose	   = E_EDITCONF_PURPOSE::setup_box;
	in.grompp1.min.emstep	   = 0.009;
	in.grompp1.min.emtol	   = 1000;
	in.grompp1.min.nsteps	   = 6000;
	in.genion.conc			   = 0.15;
	in.genion.pname			   = "K";
	in.genion.pq			   = 1;
	in.genion.nname			   = "CL";
	in.genion.nq			   = -1;

	in.rootDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareStructure" / "1ubq";
	if ( fs::exists( in.rootDir ) )
		fs::remove_all( in.rootDir );
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

TEST_CASE( "VTX_TOOL_MdPrep - prepareStructure", "[prepareStructure][2wfv]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );

	GromacsInstructions in;

	VTX::test::setForcefield( in, "amber03" );
	in.pdb2gmx.water		   = E_WATER_MODEL::tip3p;
	in.editconf1.bt			   = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.editconf1.d			   = 1.0;
	in.editconf1.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.editconf1.purpose	   = E_EDITCONF_PURPOSE::setup_box;
	in.grompp1.min.emstep	   = 0.02;
	in.grompp1.min.emtol	   = 1000;
	in.grompp1.min.nsteps	   = 30;
	in.genion.conc			   = 0.15;
	in.genion.pname			   = "K";
	in.genion.pq			   = 1;
	in.genion.nname			   = "CL";
	in.genion.nq			   = -1;

	in.rootDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareStructure" / "2wfv";
	if ( fs::exists( in.rootDir ) )
		fs::remove_all( in.rootDir );
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

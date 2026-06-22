#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tool/mdprep/backends/gromacs/editconf.hpp>
//
#include "fixture.hpp"
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <iostream>
#include <tool/mdprep/backends/gromacs/util.hpp>

namespace
{
	using namespace VTX::Tool::Mdprep;
} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - editconf - prepareJob - producing_pdb", "[prepareJob][editconf][producing_pdb]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;

	VTX::test::PrepareJobSetup f( "editconf", "8_editconf" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Genion::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Grompp::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Solvate::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Editconf::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::fileList(), previousOutputs );
	EditconfInstructions in;
	in.fileStem = "1ubq";
	in.purpose	= E_EDITCONF_PURPOSE::producing_pdb;
	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	CHECK( fs::exists( f.jobDir ) );
	CHECK( in.in.empty() == false );
	CHECK( in.in.string().ends_with( ".tpr" ) );
	CHECK( in.out.parent_path() == f.jobDir );
	CHECK( in.out.string().ends_with( "1ubq.pdb" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - prepareJob - setup_box", "[prepareJob][editconf]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;

	EditconfInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "editconf", "2_editconf" );
	prepareJob( { VTX::test::getPtrVectFromVect( VTX::test::Pdb2gmx::fileList() ) }, f.rootDir, f.jobName, in );

	CHECK( fs::exists( f.jobDir ) );
	CHECK( in.in.empty() == false );
	CHECK( in.in.string().ends_with( ".gro" ) );
	CHECK( in.out.parent_path() == f.jobDir );
	CHECK( in.out.string().ends_with( "1ubq.gro" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - empty", "[convert][editconf][empty]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	GromacsJobData		 out;
	convert( in, out );

	CHECK( out.arguments.empty() );
}

namespace
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;

	void prepare( EditconfInstructions & p_in )
	{
		p_in.in	 = ".";
		p_in.out = ".";
	}
} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - distance", "[convert][editconf][distance]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
	in.d			 = 1.2;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "dodecahedron" );
	expectedOutput.arguments.push_back( "-d" );
	expectedOutput.arguments.push_back( "1.2" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - dodecahedron", "[convert][editconf][size][dodecahedron]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.bt			 = E_EDITCONF_BOX_SHAPE::dodecahedron;
	in.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::size;
	in.box[ 0 ]		 = 12.0;
	in.box[ 1 ]		 = 13.0;
	in.box[ 2 ]		 = 14.0;
	in.angles[ 0 ]	 = 88.0; // Angles should be ignored
	in.angles[ 1 ]	 = 89.0;
	in.angles[ 2 ]	 = 90.0;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "dodecahedron" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - triclinic", "[convert][editconf][size][triclinic]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.bt			 = E_EDITCONF_BOX_SHAPE::triclinic;
	in.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::size;
	in.box[ 0 ]		 = 12.0;
	in.box[ 1 ]		 = 13.0;
	in.box[ 2 ]		 = 14.0;
	in.angles[ 0 ]	 = 88.0;
	in.angles[ 1 ]	 = 89.0;
	in.angles[ 2 ]	 = 90.0;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "triclinic" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );
	expectedOutput.arguments.push_back( "13.0" );
	expectedOutput.arguments.push_back( "14.0" );
	expectedOutput.arguments.push_back( "-angles" );
	expectedOutput.arguments.push_back( "88.0" );
	expectedOutput.arguments.push_back( "89.0" );
	expectedOutput.arguments.push_back( "90.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - cubic", "[convert][editconf][size][cubic]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.bt			 = E_EDITCONF_BOX_SHAPE::cubic;
	in.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::size;
	in.box[ 0 ]		 = 12.0;
	in.box[ 1 ]		 = 13.0;
	in.box[ 2 ]		 = 14.0;
	in.angles[ 0 ]	 = 88.0; // Angles should be ignored
	in.angles[ 1 ]	 = 89.0;
	in.angles[ 2 ]	 = 90.0;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "cubic" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - octahedron", "[convert][editconf][size][octahedron]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.bt			 = E_EDITCONF_BOX_SHAPE::octahedron;
	in.dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::size;
	in.box[ 0 ]		 = 12.0;
	in.box[ 1 ]		 = 13.0;
	in.box[ 2 ]		 = 14.0;
	in.angles[ 0 ]	 = 88.0; // Angles should be ignored
	in.angles[ 1 ]	 = 89.0;
	in.angles[ 2 ]	 = 90.0;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "octahedron" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - convert - producing_pdb", "[convert][editconf][producing_pdb]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	EditconfInstructions in;
	prepare( in );
	GromacsJobData out, expectedOutput;

	in.purpose = E_EDITCONF_PURPOSE::producing_pdb;
	expectedOutput.arguments.push_back( "editconf" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.in.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.out.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-conect" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
	CHECK( out.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf + submitGromacsJob", "[submitGromacsJob][editconf]" )
{
	VTX::test::setup_env f;
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	FilePath outputEditconfDir = VTX::App::SESSION().getApplicationDir() / "out" / "editconf" / "submitGromacsJob";

	if ( fs::exists( outputEditconfDir ) == false )
	{
		fs::create_directories( outputEditconfDir );
	}

	GromacsJobData jobData;
	jobData.arguments.push_back( "editconf" );
	jobData.arguments.push_back( "-f" );
	jobData.arguments.push_back( ( VTX::test::Pdb2gmx::outputDir() / "1ubq.gro" ).string() );
	jobData.arguments.push_back( "-o" );
	jobData.arguments.push_back( ( outputEditconfDir / "1ubq.gro" ).string() );
	setLastArgumentAsExpectedOutputFile( jobData );
	jobData.arguments.push_back( "-bt" );
	jobData.arguments.push_back( "dodecahedron" );
	jobData.arguments.push_back( "-d" );
	jobData.arguments.push_back( "1.2" );
	declareFfDirectory( VTX::Tool::Mdprep::backends::Gromacs::defaultFfDirectoryPath() );
	submitGromacsJob( VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryPath(), jobData );

	checkJobResults( jobData );

	std::string outputs;
	outputs += jobData.channelsLocker.open()->stdout_;
	outputs += jobData.channelsLocker.open()->stderr_;
	for ( auto & err : jobData.report.errors )
	{
		outputs += err + "\n";
	}
	std::cout << "Here is the output from cout : \n" << ( outputs ) << "\n";
	INFO( outputs );
	CHECK( jobData.report.errorOccured == false );
	CHECK( jobData.report.finished == true );
}

TEST_CASE( "VTX_TOOL_MdPrep - editconf - prepareJob + submitGromacsJob", "[submitGromacsJob][editconf][producing_pdb]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;

	VTX::test::PrepareJobSetup f( "editconf", "8_editconf" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Genion::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Grompp::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Solvate::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Editconf::fileList(), previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::fileList(), previousOutputs );
	EditconfInstructions in;
	in.fileStem = "1ubq";
	in.purpose	= E_EDITCONF_PURPOSE::producing_pdb;
	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	GromacsJobData jobData;
	convert( in, jobData );
	declareFfDirectory( VTX::Tool::Mdprep::backends::Gromacs::defaultFfDirectoryPath() );
	submitGromacsJob( VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryPath(), jobData );

	checkJobResults( jobData );

	std::string outputs;
	outputs += jobData.channelsLocker.open()->stdout_;
	outputs += jobData.channelsLocker.open()->stderr_;
	INFO( outputs );
	CHECK( jobData.report.errorOccured == false );
	CHECK( jobData.report.finished == true );
}

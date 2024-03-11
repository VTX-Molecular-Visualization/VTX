#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs/editconf.hpp>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][empty]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	EditconfInstructions in;
	GromacsJobData		 out;
	convert( in, out );

	CHECK( out.arguments.empty() );
}

namespace
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	void prepare( EditconfInstructions & p_in )
	{
		p_in.in	 = ".";
		p_in.out = ".";
	}
} // namespace
TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][distance]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
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
	expectedOutput.expectedOutputFilesIndexes.push_back( expectedOutput.arguments.size() - 1 );
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "dodecahedron" );
	expectedOutput.arguments.push_back( "-d" );
	expectedOutput.arguments.push_back( "1.2" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
}

TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][size][dodecahedron]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
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
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "dodecahedron" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
}
TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][size][triclinic]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
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
}
TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][size][cubic]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
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
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "cubic" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
}
TEST_CASE( "VTX_TOOL_MdPrep - convert - editconf", "[convert][editconf][size][octahedron]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
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
	expectedOutput.arguments.push_back( "-bt" );
	expectedOutput.arguments.push_back( "octahedron" );
	expectedOutput.arguments.push_back( "-box" );
	expectedOutput.arguments.push_back( "12.0" );

	convert( in, out );

	CHECK( out.arguments == expectedOutput.arguments );
}

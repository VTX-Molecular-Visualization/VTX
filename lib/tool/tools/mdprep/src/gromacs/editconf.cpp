#include <algorithm>
//
#include <tools/mdprep/gromacs/editconf.hpp>
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	std::string string( const E_EDITCONF_BOX_SHAPE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_EDITCONF_BOX_SHAPE::dodecahedron: return "dodecahedron";
		case E_EDITCONF_BOX_SHAPE::cubic: return "cubic";
		case E_EDITCONF_BOX_SHAPE::octahedron: return "octahedron";
		case E_EDITCONF_BOX_SHAPE::triclinic: return "triclinic";
		default: break;
		}
		return "";
	}

	std::string format( const double & p_ ) noexcept { return std::format( "{:.1f}", p_ ); }

	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const fs::path &			 p_root,
		const std::string_view &	 p_folderName,
		EditconfInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		auto lastGroFile = std::find_if(
			p_previousJobsOutputs.fileStringPtrs.begin(),
			p_previousJobsOutputs.fileStringPtrs.end(),
			[]( const std::string * p_ ) { return p_->ends_with( ".gro" ); }
		);
		if ( lastGroFile != std::end( p_previousJobsOutputs.fileStringPtrs ) )
			p_instructions.in = *lastGroFile.operator*();
		p_instructions.out = jobDir / ( p_instructions.fileStem + ".gro" );
	}

	void convert( const EditconfInstructions & in, GromacsJobData & out ) noexcept
	{
		if ( in.in.empty() || in.out.empty() )
			return;

		out.arguments.push_back( "editconf" );
		out.arguments.push_back( "-f" );
		out.arguments.push_back( fs::path( in.in ).make_preferred().string() );
		out.arguments.push_back( "-o" );
		out.arguments.push_back( fs::path( in.out ).make_preferred().string() );
		out.arguments.push_back( "-bt" );
		out.arguments.push_back( string( in.bt ) );
		if ( in.dimensionMode == E_EDITCONF_BOX_DIMENSION_MODE::distance )
		{
			out.arguments.push_back( "-d" );
			out.arguments.push_back( format( in.d ) );
			return;
		}
		out.arguments.push_back( "-box" );
		out.arguments.push_back( format( in.box[ 0 ] ) );
		if ( in.bt == E_EDITCONF_BOX_SHAPE::triclinic )
		{
			out.arguments.push_back( format( in.box[ 1 ] ) );
			out.arguments.push_back( format( in.box[ 2 ] ) );
			out.arguments.push_back( "-angles" );
			out.arguments.push_back( format( in.angles[ 0 ] ) );
			out.arguments.push_back( format( in.angles[ 1 ] ) );
			out.arguments.push_back( format( in.angles[ 2 ] ) );
		}
	}
} // namespace VTX::Tool::Mdprep::Gromacs

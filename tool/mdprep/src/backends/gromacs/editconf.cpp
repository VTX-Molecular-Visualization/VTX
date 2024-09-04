#include <algorithm>
#include <fmt/format.h>
//
#include <tool/mdprep/backends/gromacs/editconf.hpp>
#include <tool/mdprep/backends/gromacs/inputs.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include "tool/mdprep/backends/gromacs/util.hpp"

namespace VTX::Tool::Mdprep::backends::Gromacs
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

	std::string string( const E_EDITCONF_BOX_DIMENSION_MODE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_EDITCONF_BOX_DIMENSION_MODE::distance: return "distance";
		case E_EDITCONF_BOX_DIMENSION_MODE::size: return "size";
		default: return "";
		}
	}

	std::string format( const double & p_ ) noexcept { return fmt::format( "{:.1f}", p_ ); }

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

		if ( p_instructions.purpose == E_EDITCONF_PURPOSE::setup_box )
		{
			auto lastGroFile = std::find_if(
				p_previousJobsOutputs.fileStringPtrs.begin(),
				p_previousJobsOutputs.fileStringPtrs.end(),
				[]( const std::string * p_ ) { return p_->ends_with( ".gro" ); }
			);
			if ( lastGroFile != std::end( p_previousJobsOutputs.fileStringPtrs ) )
				p_instructions.in = *lastGroFile.operator*();
			p_instructions.out = jobDir / ( p_instructions.fileStem + ".gro" );
		}
		else
		{
			if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".tpr" ) )
			{
				p_instructions.in = *fileStrPtr;
			}
			p_instructions.out = jobDir / ( p_instructions.fileStem + ".pdb" );
		}
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
		setLastArgumentAsExpectedOutputFile( out );
		if ( in.purpose == E_EDITCONF_PURPOSE::setup_box )
		{
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
		if ( in.purpose == E_EDITCONF_PURPOSE::producing_pdb )
		{
			out.arguments.push_back( "-conect" );
		}
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs

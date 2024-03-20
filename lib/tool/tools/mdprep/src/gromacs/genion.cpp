#include <tools/mdprep/gromacs/genion.hpp>
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>
#include <tools/mdprep/gromacs/util.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const fs::path &			 p_root,
		const std::string_view &	 p_folderName,
		GenionInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".tpr" ) )
		{
			p_instructions.inputTpr = *fileStrPtr;
		}
		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".top" ) )
		{
			fs::path topFile		= *fileStrPtr;
			p_instructions.inputTop = jobDir / ( p_instructions.fileStem + ".top" );

			if ( fs::exists( topFile ) )
				fs::copy( topFile, p_instructions.inputTop );
		}
		p_instructions.outputGro = jobDir / ( p_instructions.fileStem + ".gro" );
	}
	void convert( const GenionInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "genion" );
		p_out.arguments.push_back( "-s" );
		p_out.arguments.push_back( p_in.inputTpr.string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( p_in.inputTop.string() );
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputGro.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-pname" );
		p_out.arguments.push_back( p_in.pname );
		p_out.arguments.push_back( "-pq" );
		p_out.arguments.push_back( std::to_string( p_in.pq ) );
		p_out.arguments.push_back( "-nname" );
		p_out.arguments.push_back( p_in.nname );
		p_out.arguments.push_back( "-nq" );
		p_out.arguments.push_back( std::to_string( p_in.pq ) );
		p_out.arguments.push_back( "-neutral" );
		if ( p_in.conc.has_value() )
		{
			p_out.arguments.push_back( "-conc" );
			p_out.arguments.push_back( std::to_string( p_in.conc.value() ) );
		}
	}
} // namespace VTX::Tool::Mdprep::Gromacs

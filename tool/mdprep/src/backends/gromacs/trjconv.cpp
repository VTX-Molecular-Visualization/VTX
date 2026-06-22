#include <algorithm>
#include <tool/mdprep/backends/gromacs/trjconv.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const FilePath &			 p_root,
		const std::string_view &	 p_folderName,
		TrjconvInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		FilePath jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		auto lastGroFile = std::find_if(
			p_previousJobsOutputs.fileStringPtrs.begin(),
			p_previousJobsOutputs.fileStringPtrs.end(),
			[]( const std::string * p_ ) { return p_->ends_with( ".gro" ); }
		);
		if ( lastGroFile != std::end( p_previousJobsOutputs.fileStringPtrs ) )
		{
			p_instructions.inputGro = *lastGroFile.operator*();
		}
		p_instructions.outputGro = jobDir / ( p_instructions.fileStem + ".gro" );
	}

	void convert( const TrjconvInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "trjconv" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-s" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputGro.string() );
		p_out.expectedOutputFilesIndexes.push_back( p_out.arguments.size() - 1 );
		p_out.arguments.push_back( "-center" );
		p_out.arguments.push_back( "-center-group" );
		p_out.arguments.push_back( "Protein" );
		p_out.arguments.push_back( "-output-group" );
		p_out.arguments.push_back( "System" );
		p_out.arguments.push_back( "-pbc" );
		p_out.arguments.push_back( "res" );
		p_out.arguments.push_back( "-ur" );
		p_out.arguments.push_back( "compact" );
	}

} // namespace VTX::Tool::Mdprep::backends::Gromacs

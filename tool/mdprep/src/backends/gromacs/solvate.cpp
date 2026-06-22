#include <algorithm>
#include <tool/mdprep/backends/gromacs/solvate.hpp>
#include <util/logger.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include "tool/mdprep/backends/gromacs/util.hpp"

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const FilePath &			 p_root,
		const std::string_view &	 p_folderName,
		SolvateInstructions &		 p_instructions
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
		else
		{
			VTX_ERROR( "[MDPREP] Solvate input structure was not found in previous job outputs." );
		}

		p_instructions.inputTop	   = jobDir / ( p_instructions.fileStem + ".top" );
		const FilePath & sourceTop = p_previousJobsOutputs.lastUncompiledTop;

		if ( sourceTop.empty() || fs::is_regular_file( sourceTop ) == false )
		{
			VTX_ERROR(
				"[MDPREP] Solvate input topology <{}> is missing before copy.",
				sourceTop.empty() ? "<empty>" : sourceTop.string()
			);
		}
		else
		{
			std::error_code copyError;
			fs::copy_file( sourceTop, p_instructions.inputTop, fs::copy_options::overwrite_existing, copyError );
			if ( copyError )
			{
				VTX_ERROR(
					"[MDPREP] Unable to copy solvate topology from <{}> to <{}>: {}",
					sourceTop.string(),
					p_instructions.inputTop.string(),
					copyError.message()
				);
			}
			else
			{
				VTX_INFO(
					"[MDPREP] Solvate topology copied from <{}> to <{}> ({} bytes).",
					sourceTop.string(),
					p_instructions.inputTop.string(),
					fs::file_size( p_instructions.inputTop )
				);
			}
		}

		p_instructions.outputGro = jobDir / ( p_instructions.fileStem + ".gro" );
	}

	void convert( const SolvateInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "solvate" );
		p_out.arguments.push_back( "-cs" );
		p_out.arguments.push_back( "spc216.gro" );
		p_out.arguments.push_back( "-cp" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( p_in.inputTop.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputGro.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
	}

} // namespace VTX::Tool::Mdprep::backends::Gromacs

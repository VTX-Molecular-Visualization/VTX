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
	void convert( const GenionInstructions &, GromacsJobData & ) noexcept {}
} // namespace VTX::Tool::Mdprep::Gromacs

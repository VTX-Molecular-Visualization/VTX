#include <tools/mdprep/gromacs/inputs.hpp>
#include <tools/mdprep/gromacs/solvate.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const fs::path &			 p_root,
		const std::string_view &	 p_folderName,
		SolvateInstructions &		 p_instructions
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
			p_instructions.inputGro = *lastGroFile.operator*();
		auto lastTopFile = std::find_if(
			p_previousJobsOutputs.fileStringPtrs.begin(),
			p_previousJobsOutputs.fileStringPtrs.end(),
			[]( const std::string * p_ ) { return p_->ends_with( ".top" ); }
		);
		if ( lastGroFile != std::end( p_previousJobsOutputs.fileStringPtrs ) )
			p_instructions.inputTop = *lastGroFile.operator*();
		p_instructions.outputGro = jobDir / ( p_instructions.fileStem + ".gro" );
	}

	void convert( const SolvateInstructions &, GromacsJobData & ) noexcept {}

} // namespace VTX::Tool::Mdprep::Gromacs

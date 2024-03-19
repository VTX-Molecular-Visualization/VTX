#include <tools/mdprep/gromacs/grompp.hpp>
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
		GromppInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		// grompp takes .top as strict input and doesn't touch it, I believe. So no need to copy it
		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".top" ) )
		{
			p_instructions.inputTop = *fileStrPtr;
		}
	}
} // namespace VTX::Tool::Mdprep::Gromacs

#include <tools/mdprep/gromacs/gromacs.hpp>
//
#include <tools/mdprep/gromacs/solvate.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, SolvateInstructions & ) noexcept
	{
	}

	void convert( const SolvateInstructions &, GromacsJobData & ) noexcept {}

} // namespace VTX::Tool::Mdprep::Gromacs

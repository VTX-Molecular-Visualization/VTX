#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <array>
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>
//
#include <tools/mdprep/gromacs/inputs.hpp>
#include <tools/mdprep/gromacs/pdb2gmx.hpp>
#include <util/datalocker.hpp>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep
{
	// Return the absolute path of the directory containing VTX executable
	const fs::path & executableDirectory() noexcept;

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	// Return the position of default forcefields packaged with vtx, relative to the vtx executable folder
	const fs::path & defaultFfDirectoryRelativePath() noexcept;

	// Return the position of gmx binary relative to the vtx executable folder
	const fs::path & defaultGmxBinaryRelativePath() noexcept;

	// Show gromacs where to look for forcefields
	void declareFfDirectory( const fs::path & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

namespace VTX::Tool::Mdprep::Gromacs
{

	struct solvateInstructions // WIP // TODO : move it in appropriate location
	{
		fs::path				 inputGro;
		fs::path				 outputGro;
		std::array<double, 3ull> box { 0., 0., 0. };
		double					 radius = 0.105;
		double					 scale	= 0.57;
		double					 shell	= 0.;
		int						 maxsol = 0;
		bool					 vel	= false;
	};

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	//    If the output_gro is empty, will use the input filename root and append "solv"
	void convert( const solvateInstructions &, GromacsCommandArgs & ) noexcept;
} // namespace VTX::Tool::Mdprep::Gromacs

#endif

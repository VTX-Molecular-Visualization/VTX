#ifndef __VTX_TOOL_TOOLS_EDITCONF_GROMACS__
#define __VTX_TOOL_TOOLS_EDITCONF_GROMACS__

#include <array>
#include <filesystem>
#include <string_view>
namespace fs = std::filesystem;

class QProcess;

namespace VTX::Tool::Mdprep::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	enum class E_EDITCONF_BOX_SHAPE
	{
		dodecahedron,
		octahedron,
		cubic,
		triclinic,
	};
	enum class E_EDITCONF_BOX_DIMENSION_MODE
	{
		distance // uses the distance from the solute to the box to determine box's size
		,
		size // uses user-provided dimensions for the box size.
	};
	struct EditconfInstructions
	{
		fs::path					  in;
		fs::path					  out;
		std::string					  fileStem;
		E_EDITCONF_BOX_SHAPE		  bt			= E_EDITCONF_BOX_SHAPE::dodecahedron;
		E_EDITCONF_BOX_DIMENSION_MODE dimensionMode = E_EDITCONF_BOX_DIMENSION_MODE::distance;
		double						  d				= 1.;
		std::array<double, 3>		  box { 0., 0., 0. };
		std::array<double, 3>		  angles { 90., 90., 90. }; // can't be used in distance mode
	};

	// Setup job folder and place input files as the job need them
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, EditconfInstructions & ) noexcept;

	// Convert editconf instructions to gromacs command
	//  Do not perform filesystem check
	//  Does nothing if in or out is empty
	void convert( const EditconfInstructions &, GromacsJobData & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

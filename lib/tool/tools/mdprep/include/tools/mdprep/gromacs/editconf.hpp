#ifndef __VTX_TOOL_TOOLS_EDITCONF_GROMACS__
#define __VTX_TOOL_TOOLS_EDITCONF_GROMACS__

#include <array>
#include <filesystem>
#include <string_view>
namespace fs = std::filesystem;

class QProcess;

namespace VTX::Tool::Mdprep::Gromacs
{
	struct GromacsCommandArgs;

	enum class E_BOX_SHAPE
	{
		none,
		dodecahedron,
		octahedron,
		cubic,
		triclinic,
	};

	struct EditconfInstructions
	{
		fs::path			  in;
		fs::path			  out;
		double				  d;
		std::array<double, 3> box;
		std::array<double, 3> angles;
	};

	void convert( const EditconfInstructions &, GromacsCommandArgs & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

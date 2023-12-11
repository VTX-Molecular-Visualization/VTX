#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep
{
	// Show gromacs where to look for forcefields
	void declare_ff_directory( const fs::path & ) noexcept;

	// Return the position of default forcefields packaged with vtx, relative to the vtx executable folder
	const fs::path & default_ff_directory_relative_path() noexcept;

	// Return the absolute path of the directory containing VTX executable
	const fs::path & executable_directory() noexcept;

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::gromacs
{

	struct forcefield
	{
		std::string		 forcefield_folder_path; // Absolute path of the forcefield folder. Type is as string so string_view can be use as a name.
		std::string_view name;					 // Name to show to the user and to give gromacs
	};

	// List forcefields available in the input directory.
	//    Throws VTX::IOException if directory is not found or input is not a directory
	//    Return an empty list if no forcefields are found.
	//    forcefields will be detected by checking child directories with pattern [forcefield name].ff
	std::vector<forcefield> list_forcefields( const fs::path & );

	enum class water_model
	{
		none,
		spc,
		spce,
		tip3p,
		tip4p,
		tip5p,
		tips3p
	};

	struct pdb2gmx_command
	{
		size_t		forcefield_index = SIZE_MAX; // position of the forcefield to use for
		water_model water			 = water_model::tip3p; 
		fs::path	output_dir;
		std::string root_file_name;
	};

} // namespace VTX::Tool::Mdprep::gromacs

#endif

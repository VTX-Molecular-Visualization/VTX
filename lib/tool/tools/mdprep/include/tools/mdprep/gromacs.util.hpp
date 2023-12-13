#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <array>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep
{
	// Return the absolute path of the directory containing VTX executable
	const fs::path & executable_directory() noexcept;

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	// Return the position of default forcefields packaged with vtx, relative to the vtx executable folder
	const fs::path & default_ff_directory_relative_path() noexcept;

	// Show gromacs where to look for forcefields
	void declare_ff_directory( const fs::path & ) noexcept;

	struct forcefield
	{
		std::string forcefield_folder_path; // Absolute path of the forcefield folder. Type is as string so string_view
											// can be use as a name.
		std::string_view name;				// Name to show to the user and to give gromacs
	};

	// List forcefields available in the input directory.
	//    Throws VTX::IOException if directory is not found or input is not a directory
	//    Return an empty list if no forcefields are found.
	//    Forcefields will be detected by checking child directories with pattern [forcefield name].ff
	//    Therefore, if there is some folders with this pattern in the input directory, this function will return
	//    misleading forcefield object
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

	struct pdb2gmx_instructions
	{
		std::vector<forcefield> forcefields;
		size_t					forcefield_index = SIZE_MAX; // position of the forcefield to use for
		fs::path				output_dir;
		std::string				root_file_name;
		fs::path				input_pdb;

		water_model water = water_model::tip3p;
	};
	struct gromacs_command_args;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does nothing if the forcefield_index refer to a non-existing forcefield
	//    Does nothing if the input pdb doesn't exists
	//    If the output_dir is empty, will create a output directory in the current working directory and use it.
	void convert( const pdb2gmx_instructions &, gromacs_command_args & ) noexcept;

	struct solvate_instructions // WIP
	{
		fs::path				 input_gro;
		fs::path				 output_gro;
		std::array<double, 3ull> box { 0., 0., 0. };
		double					 radius = 0.105;
		double					 scale	= 0.57;
		double					 shell	= 0.;
		int						 maxsol = 0;
		bool					 vel	= false;
	};

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does nothing if the input pdb doesn't exists
	//    If the output_dir is empty, will use the input filename root and append "solv"
	void convert( const solvate_instructions &, gromacs_command_args & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

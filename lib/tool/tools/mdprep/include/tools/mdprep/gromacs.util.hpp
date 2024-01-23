#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <array>
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>
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

	// Return the position of gmx binary relative to the vtx executable folder
	const fs::path & default_gmx_binary_relative_path() noexcept;

	// Show gromacs where to look for forcefields
	void declare_ff_directory( const fs::path & ) noexcept;

	struct forcefield
	{
		std::string forcefield_folder_path; // Absolute path of the forcefield folder. Type is as string so string_view
											// can be use as a name.
		std::string_view get_name() const;	// Name to show to the user and to give gromacs
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
	const char * string( const water_model & ) noexcept;

	// Meant to refer to one of the interactive gromacs option
	enum class interactive_keyword
	{
		none,
		ss,
		ter,
		lys,
		arg,
		asp,
		glu,
		gln,
		his,
		COUNT
	};
	const char * string( const interactive_keyword & ) noexcept;

	// Meant to uniquely identify a specific instance of input required by gromacs
	struct interactiveId
	{
		char				chain = 0x00i8; // Value of 0x00 means any chain
		interactive_keyword kw	  = interactive_keyword::none;
		uint32_t			num	  = 0; // TODO : test TER and SS to see if keyword and number can apply to those

		bool operator==( const interactiveId & ) const noexcept = default;
	};
} // namespace VTX::Tool::Mdprep::Gromacs

// We implement our specialization of the hash structure for interactiveId as required for set and map
// We got to declare the hash template specialisation before declaring its consumer
namespace std
{
	template<>
	struct hash<VTX::Tool::Mdprep::Gromacs::interactiveId>
	{
		inline uint64_t operator()( const VTX::Tool::Mdprep::Gromacs::interactiveId & p_arg ) const noexcept
		{
			uint64_t out = 0;
			out			 = static_cast<uint64_t>( p_arg.kw ) << 32;
			out |= p_arg.num | ( static_cast<uint32_t>( p_arg.chain ) << 24 );
			return out;
		}
	};
} // namespace std

namespace VTX::Tool::Mdprep::Gromacs
{

	// organized version of the arguments to be used during interactive gromacs step
	struct InteractiveArguments
	{
		std::unordered_map<interactiveId, std::string> kw_v;
		bool operator==( const InteractiveArguments & ) const noexcept = default;
	};
	struct pdb2gmx_instructions
	{
		std::vector<forcefield>				 forcefields;
		size_t								 forcefield_index = SIZE_MAX; // position of the forcefield to use for
		fs::path							 output_dir;
		std::string							 root_file_name;
		fs::path							 input_pdb;
		std::optional<InteractiveArguments> custom_parameter; // needed for adding -his ...

		water_model water = water_model::tip3p;
	};
	struct GromacsCommandArgs;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does nothing if the forcefield_index refer to a non-existing forcefield
	//    Does not perform filesystem check on input pdb
	//    If the output_dir is empty, will create a output directory in the current working directory and use it.
	void convert( const pdb2gmx_instructions &, GromacsCommandArgs & ) noexcept;

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
	//    Does not perform filesystem check on input gro
	//    If the output_gro is empty, will use the input filename root and append "solv"
	void convert( const solvate_instructions &, GromacsCommandArgs & ) noexcept;

	// Returned by the script parser to inform how the parsing went
	struct parse_report
	{
		bool		error	= false;
		bool		warning = false;
		std::string message;
	};

	// TODO : find a pdb struct that allow SS and TER interactive and check if that format is still relevent
	// Parse user-provided script that specify interactive gromacs behavior
	//  format should be the following :
	//    [chain] [kw][num] [value]
	//  for example :
	//    A HIS82 HISE
	//    B ARG1 0
	//  one line = one argument (arguments separated with a newline)
	//  space between num and value can be any number of white space or tab
	parse_report parse_pdb2gmx_user_script( const std::string_view &, InteractiveArguments & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <array>
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <util/datalocker.hpp>
#include <vector>
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

	struct forcefield
	{
		std::string forcefieldFolderPath; // Absolute path of the forcefield folder. Type is as string so string_view
										  // can be use as a name.
		std::string_view getName() const; // Name to show to the user and to give gromacs
	};

	// List forcefields available in the input directory.
	//    Throws VTX::IOException if directory is not found or input is not a directory
	//    Return an empty list if no forcefields are found.
	//    Forcefields will be detected by checking child directories with pattern [forcefield name].ff
	//    Therefore, if there is some folders with this pattern in the input directory, this function will return
	//    misleading forcefield object
	std::vector<forcefield> listForcefields( const fs::path & );

	enum class E_WATER_MODEL
	{
		none,
		spc,
		spce,
		tip3p,
		tip4p,
		tip5p,
		tips3p
	};
	const char * string( const E_WATER_MODEL & ) noexcept;

	// Meant to refer to one of the interactive gromacs option
	enum class E_INTERACTIVE_KEYWORD
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
	const char * string( const E_INTERACTIVE_KEYWORD & ) noexcept;

	// Meant to uniquely identify a specific instance of input required by gromacs
	struct Pdb2gmxInputId
	{
		char				  chain = 0x00i8; // Value of 0x00 means any chain
		E_INTERACTIVE_KEYWORD kw	= E_INTERACTIVE_KEYWORD::none;
		uint32_t			  num	= 0; // TODO : test TER and SS to see if keyword and number can apply to those

		bool operator==( const Pdb2gmxInputId & ) const noexcept = default;
	};
} // namespace VTX::Tool::Mdprep::Gromacs

// We implement our specialization of the hash structure for Pdb2gmxInputId as required for set and map
// We got to declare the hash template specialisation before declaring its consumer
namespace std
{
	template<>
	struct hash<VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId>
	{
		inline uint64_t operator()( const VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId & p_arg ) const noexcept
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
	struct Pdb2gmxInputs
	{
		std::unordered_map<Pdb2gmxInputId, std::string> kwValue;
		bool											operator==( const Pdb2gmxInputs & ) const noexcept = default;
	};
	struct Pdb2gmxInstructions
	{
		std::vector<forcefield>		 forcefields;
		size_t						 forcefieldIndex = SIZE_MAX; // position of the forcefield to use for
		fs::path					 outputDir;
		std::string					 rootFileName;
		fs::path					 inputPdb;
		std::optional<Pdb2gmxInputs> customParameter; // needed for adding -his ...

		E_WATER_MODEL water = E_WATER_MODEL::tip3p;
	};

	struct Channels
	{
		std::string stdout_;
		std::string stderr_;
	};

	// This class is responsible for unfolding gromacs interactive process by entering interactive input when gromacs
	// expect it.
	class Inputs // TODO : type-erased class for interactive input (cover Pdb2gmxInput and the ones for editconf,
				 // trjconv and genion)
	{
	};
	struct GromacsCommandArgs
	{
		std::vector<std::string>		arguments;
		VTX::Util::DataLocker<Channels> channels;
		std::optional<Inputs>
			 interactiveSettings; // If the Inputs class is instanciated, the process is expected to be interactive.
		bool operator==( const GromacsCommandArgs & ) const noexcept = default;
	};

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does nothing if the forcefieldIndex refer to a non-existing forcefield
	//    Does not perform filesystem check on input pdb
	//    If the outputDir is empty, will create a output directory in the current working directory and use it.
	void convert( const Pdb2gmxInstructions &, GromacsCommandArgs & ) noexcept;

	struct solvateInstructions // WIP
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

	// Returned by the script parser to inform how the parsing went
	struct parseReport
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
	parseReport parsePdb2gmxUserScript( const std::string_view &, Pdb2gmxInputs & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

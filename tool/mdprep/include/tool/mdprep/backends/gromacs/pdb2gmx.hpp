#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_PDB2GMX__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_PDB2GMX__

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace fs = std::filesystem;

class QProcess;

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	struct GromacsJobData;

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
		tip3p,
		spc,
		spce,
		tip4p,
		tip5p,
		tips3p,
		none,
		COUNT
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
} // namespace VTX::Tool::Mdprep::backends::Gromacs

// We implement our specialization of the hash structure for Pdb2gmxInputId as required for set and map
// We got to declare the hash template specialisation before declaring its consumer
namespace std
{
	template<>
	struct hash<VTX::Tool::Mdprep::backends::Gromacs::Pdb2gmxInputId>
	{
		inline uint64_t operator()( const VTX::Tool::Mdprep::backends::Gromacs::Pdb2gmxInputId & p_arg ) const noexcept
		{
			uint64_t out = 0;
			out			 = static_cast<uint64_t>( p_arg.kw ) << 32;
			out |= p_arg.num | ( static_cast<uint32_t>( p_arg.chain ) << 24 );
			return out;
		}
	};
} // namespace std
namespace VTX::Tool::Mdprep::backends::Gromacs
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
		std::string					 fileStem;
		fs::path					 inputPdb;
		std::optional<Pdb2gmxInputs> customParameter; // needed for adding -his ...

		E_WATER_MODEL water = E_WATER_MODEL::tip3p;
	};

	// Returned by the script parser to inform how the parsing went
	struct parseReport
	{
		bool		error	= false;
		bool		warning = false;
		std::string message;
	};
	struct CumulativeOuputFiles;

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

	// Setup job folder and place input files as the job need them
	// The root is assumed not to be an existing file path.
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, Pdb2gmxInstructions & ) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does nothing if the forcefieldIndex refer to a non-existing forcefield
	//    Does not perform filesystem check on input pdb
	//    If the outputDir is empty, will create a output directory in the current working directory and use it.
	void convert( const Pdb2gmxInstructions &, GromacsJobData & ) noexcept;

	bool		isWaitingForInput( const Pdb2gmxInputs &, const std::string_view & p_stdout ) noexcept;
	inline bool enterInput(
		const Pdb2gmxInputs &,
		QProcess &,
		std::string & p_stdout,
		std::string & p_stderr
	) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif

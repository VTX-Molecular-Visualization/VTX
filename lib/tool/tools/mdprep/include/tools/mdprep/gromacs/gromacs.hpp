#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <array>
#include <tools/mdprep/gromacs/editconf.hpp>
#include <tools/mdprep/gromacs/genion.hpp>
#include <tools/mdprep/gromacs/grompp.hpp>
#include <tools/mdprep/gromacs/job.hpp>
#include <tools/mdprep/gromacs/pack.hpp>
#include <tools/mdprep/gromacs/pdb2gmx.hpp>
#include <tools/mdprep/gromacs/solvate.hpp>
#include <tools/mdprep/gromacs/trjconv.hpp>
#include <tools/mdprep/gromacs/util.hpp>

namespace std
{
	class stop_token;
}

namespace VTX::Tool::Mdprep::Gromacs
{
	const uint8_t g_NUM_PREPARATION_JOBS = 8ui8;
	struct GromacsInstructions
	{
		Pdb2gmxInstructions	 pdb2gmx;
		EditconfInstructions editconf1;
		SolvateInstructions	 solvate;
		TrjconvInstructions	 trjconv;
		GromppInstructions	 grompp1;
		GenionInstructions	 genion;
		GromppInstructions	 grompp2;
		EditconfInstructions editconf2 { .purpose = E_EDITCONF_PURPOSE::producing_pdb };

		std::string										   fileStem;
		std::array<GromacsJobData, g_NUM_PREPARATION_JOBS> jobData;
		CumulativeOuputFiles							   outputs;
		fs::path										   rootDir = createNewEmptyTempDirectory();
	};

	// Use input structure and instuction to make the structure ready for MD simulations.
	// Assumes gromacs context has been set beforehand
	void prepareStructure( std::stop_token &, const fs::path & p_structurePdb, GromacsInstructions & ) noexcept;

	// Use prepared structure to deliver a ready-to-use directory to start MD from
	void createMdDirectory( const GromacsInstructions &, const fs::path & p_dest ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

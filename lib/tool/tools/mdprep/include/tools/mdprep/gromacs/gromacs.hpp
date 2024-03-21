#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include <tools/mdprep/gromacs/editconf.hpp>
#include <tools/mdprep/gromacs/genion.hpp>
#include <tools/mdprep/gromacs/grompp.hpp>
#include <tools/mdprep/gromacs/job.hpp>
#include <tools/mdprep/gromacs/pack.hpp>
#include <tools/mdprep/gromacs/pdb2gmx.hpp>
#include <tools/mdprep/gromacs/solvate.hpp>
#include <tools/mdprep/gromacs/trjconv.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{

	struct GromacsInstructions
	{
		CumulativeOuputFiles outputs;
		Pdb2gmxInstructions	 pdb2gmx;
		EditconfInstructions editconf1;
		SolvateInstructions	 solvate;
		TrjconvInstructions	 trjconv;
		GromppInstructions	 grompp1;
		GenionInstructions	 genion;
		GromppInstructions	 grompp2;
		EditconfInstructions editconf2;
	};

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

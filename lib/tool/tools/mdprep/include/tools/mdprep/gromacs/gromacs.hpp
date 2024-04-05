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
	const uint8_t g_NUM_PREPARATION_JOBS = 9ui8;
	struct GromacsInstructions
	{
		Pdb2gmxInstructions	 pdb2gmx;
		EditconfInstructions editconf1;
		SolvateInstructions	 solvate;
		TrjconvInstructions	 trjconv;
		GromppInstructions	 gromppIons;
		GenionInstructions	 genion;
		GromppInstructions	 gromppPosres;
		GromppInstructions	 gromppEm;
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

	// Class responsible for testing the system with gromacs asynchronously.
	class SystemTester
	{
	  public:
		SystemTester( const fs::path & p_structurePdb, const forcefield &, const E_WATER_MODEL & );

		// Return if the asynchronous test is over. Usefull if you don't want to block the current thread waiting
		bool isTestFinished() const noexcept;

		// Return weither the system is accepted by gromacs or not. Wait for the test to finish before returning the
		// value, potentially blocking the current thread.
		bool isSystemOk() const noexcept;

		// Return weither the system is accepted by gromacs or not. Wait for the test to finish before returning the
		// value, potentially blocking the current thread.
		operator bool() const noexcept;

	  private:
		class _Impl;
		std::unique_ptr<_Impl> _pimpl = nullptr;
	};

} // namespace VTX::Tool::Mdprep::Gromacs

#endif

#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_FORM_DATA__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_FORM_DATA__

#include <any>
#include <optional>
#include <string>
#include <vector>

class QFormLayout;

namespace VTX::Tool::Mdprep::Gateway
{
	// Data structure meant to be a channel of communication from the MD engine to the common form. Therefore it shall
	// be const from the common form perspective.
	// It will contain all data that are needed by the common form, but are specific to the engine (e.g. the forcefield)
	struct EngineSpecificCommonFormData
	{
		std::vector<std::string>
			bioForcefieldCollection; // String to show users to choose the forcefield used in biological compounds.
									 // Order of these elements should be meaningfull.
	};

	// Class responsible for framing data only understood by the engine itself
	class EngineSpecificFormData
	{
		std::any formData;
	};

	enum class E_MD_DURATION_UNIT
	{
		femtoSeconds,
		picoSeconds,
		nanoSeconds,
		microSeconds,
		COUNT
	};
	const char * string( const E_MD_DURATION_UNIT & ) noexcept;

	// Return the multiplier to convert the number of MD step to a picosecond time
	const double multiplierPicoSeconds( const E_MD_DURATION_UNIT & ) noexcept;

	// Hold data for one MD step. In this context, a step can be equilibration nvt,npt or production run
	struct MdParametersStepData
	{
		uint64_t nsteps		  = 50000000;
		double	 dt			  = 0.002;
		uint64_t saveInterval = 100;
	};
	struct MinimizationData
	{
		double	 energyBreakpoint = 1000.0;
		double	 stepSize		  = 0.01;
		uint64_t stepNumber		  = 50000;
	};

	// Datastructure needed for backend communication
	struct MdParameters
	{
		std::string forcefieldBio;
		std::string forcefieldChem; // ignored for now

		MinimizationData	 minimization;
		MdParametersStepData nvt;
		MdParametersStepData npt;
		MdParametersStepData prod { .nsteps = 250000000 };
		// TODO : migrate prodNstxout_compressed towards gromacs param
		uint64_t	prodNstxout_compressed = 100;
		std::string anionName { "Cl" };
		int8_t		anionCharge = -1;
		std::string cationName { "K" };
		int8_t		cationCharge = +1;
	};
	struct MdBasicDataSample
	{
		double			   durationEquilibrationNvt = 100.0;
		E_MD_DURATION_UNIT unitEquilibrationNvt		= E_MD_DURATION_UNIT::nanoSeconds;
		double			   durationEquilibrationNpt = 100.0;
		E_MD_DURATION_UNIT unitEquilibrationNpt		= E_MD_DURATION_UNIT::nanoSeconds;
		double			   durationProduction		= 500.0;
		E_MD_DURATION_UNIT unitProduction			= E_MD_DURATION_UNIT::nanoSeconds;
	};
	// Data structure meant to update basic parameters form static informations using data from advanced parameters
	struct MdAdvancedDataSample
	{
		double	 dtNvt			= 0.002;
		uint64_t stepNumberNvt	= 500000000;
		double	 dtNpt			= 0.002;
		uint64_t stepNumberNpt	= 500000000;
		double	 dtProd			= 0.002;
		uint64_t stepNumberProd = 25000000000;
	};

} // namespace VTX::Tool::Mdprep::Gateway

#endif

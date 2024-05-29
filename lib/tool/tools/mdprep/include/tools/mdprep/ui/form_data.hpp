#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_DATA__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_DATA__

#include <optional>
#include <string>
#include <vector>

class QFormLayout;

namespace VTX::Tool::Mdprep::ui
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

	struct FormLayouts
	{
		QFormLayout * baseParameters					 = nullptr;
		QFormLayout * advancedParameters				 = nullptr;
		QFormLayout * advancedParametersSystemPrepartion = nullptr;
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
	struct MdParametersBasic
	{
		std::string		   forcefieldBio;
		std::string		   forcefieldChem; // ignored for now
		uint64_t		   stepNumberMinimization		 = static_cast<uint64_t>( 10e5 );
		uint64_t		   DurationValueEquilibrationNvt = 100;
		E_MD_DURATION_UNIT DurationUnitNvt				 = E_MD_DURATION_UNIT::nanoSeconds;
		uint64_t		   DurationValueEquilibrationNpt = 100;
		E_MD_DURATION_UNIT DurationUnitNpt				 = E_MD_DURATION_UNIT::nanoSeconds;
		uint64_t		   DurationValueProduction		 = 500;
		E_MD_DURATION_UNIT DurationUnitProd				 = E_MD_DURATION_UNIT::nanoSeconds;
		std::string		   anionName { "Cl" };
		int8_t			   anionCharge = -1;
		std::string		   cationName { "K" };
		int8_t			   cationCharge = +1;
	};
	struct MdParametersAdvanced
	{
		double	 minimiztionEmtol  = 1000.0;
		double	 minimiztionEmstep = 0.01;
		uint64_t minimiztionNsteps = 50000;

		struct StepData
		{
			uint64_t nsteps	   = 50000000;
			double	 dt		   = 0.002;
			uint64_t nstxout   = 100;
			uint64_t nstvout   = 1000;
			uint64_t nstenergy = 1000;
			uint64_t nstlog	   = 1000;
		};
		StepData nvt;
		StepData npt;
		StepData prod { .nsteps = 250000000 };
		uint64_t prodNstxout_compressed = 100;
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

} // namespace VTX::Tool::Mdprep::ui

#endif

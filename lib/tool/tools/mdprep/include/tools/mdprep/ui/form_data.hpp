#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_DATA__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_DATA__

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
		QFormLayout * baseParameters	 = nullptr;
		QFormLayout * advancedParameters = nullptr;
	};

	struct MdParametersBasic
	{
		std::string forcefieldBio;
		std::string forcefieldChem; // ignored for now
		uint64_t	stepNumberMinimization	   = static_cast<uint64_t>( 10e5 );
		uint64_t	stepNumberEquilibrationNvt = static_cast<uint64_t>( 10e5 );
		uint64_t	stepNumberEquilibrationNpt = static_cast<uint64_t>( 10e5 );
		uint64_t	stepNumberProduction	   = static_cast<uint64_t>( 2.5e8 );
		std::string anionName { "Cl" };
		int8_t		anionCharge = -1;
		std::string cationName { "K" };
		int8_t		cationCharge = +1;
	};
	struct MdParametersAdvanced
	{
	};
	// Data structure meant to update basic parameters form static informations using data from advanced parameters
	struct MdAdvancedDataSample
	{
		double dt = 0.002;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif

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
	};
	struct MdParametersAdvanced
	{
	};

} // namespace VTX::Tool::Mdprep::ui

#endif

#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__

class QFormLayout;

namespace VTX::Tool::Mdprep::ui
{

	// Datastructure allowing engine specific fields to be placed with regard for the form
	struct FormLayouts
	{
		QFormLayout * baseParameters					 = nullptr;
		QFormLayout * advancedParametersSystemPrepartion = nullptr;
		QFormLayout * advancedParametersMinimization	 = nullptr;
		QFormLayout * advancedParametersEquilibrationNvt = nullptr;
		QFormLayout * advancedParametersEquilibrationNpt = nullptr;
		QFormLayout * advancedParametersProduction		 = nullptr;
	};

	// Fields are grouped into the following sections
	enum class E_FIELD_SECTION
	{
		system,
		minimization,
		equilibrationNvt,
		equilibrationNpt,
		production,
		ions
	};
} // namespace VTX::Tool::Mdprep::ui
#endif

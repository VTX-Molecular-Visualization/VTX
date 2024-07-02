#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__

class QFormLayout;
class QWidget;
class QComboBox;

namespace VTX::Tool::Mdprep::ui
{

	// Datastructure allowing engine specific fields to be placed with regard for the form
	struct FormLayouts
	{
		QFormLayout * baseParametersIon					 = nullptr;
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

	enum class E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD
	{
		bioForcefield,
		boxShape,
		boxDimensionMode,
		waterModel,
		COUNT
	};
	struct EngineSpecificCommonInformationFieldUpdate
	{
		E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD field	= E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD::bioForcefield;
		size_t									   newValue = 0xffffffffffffffff;
	};
	enum class E_COMMON_FIELDS_COMBOBOX
	{
		bioForcefield,
		chemForcefield,
		boxShape,
		waterModel,
		COUNT
	};
	void get( const E_COMMON_FIELDS_COMBOBOX &, QWidget *& label, QComboBox *& field ) noexcept;

} // namespace VTX::Tool::Mdprep::ui
#endif

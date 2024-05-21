#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

#include <qlineedit.h>

namespace VTX::Tool::Mdprep::ui
{
	struct MdParametersBasic;

	// Class responsible for setting up the form for basic MD parameters
	class MdBasicParamForm
	{
		QFormLayout * _layoutFields				   = nullptr;
		QLineEdit *	  _fieldMinimizationStepNumber = nullptr;

	  public:
		MdBasicParamForm() = default;

		void setupUi( QWidget * p_container );

		void get( MdParametersBasic & ) const noexcept;

		QFormLayout * layoutFieldsMdEngine() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui
#endif

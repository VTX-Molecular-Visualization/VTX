#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

#include <qlineedit.h>

namespace VTX::Tool::Mdprep::ui
{
	struct MdParametersBasic;

	// Class responsible for setting up the form for basic MD parameters
	class MdBasicParamForm
	{
		QFormLayout * _layoutFields = nullptr;

		QComboBox * _fieldForceFieldBio	 = nullptr;
		QComboBox * _fieldForceFieldChem = nullptr;

		QLineEdit * _fieldStepNumberMinimization	 = nullptr;
		QLineEdit * _fieldStepNumberEquilibrationNvt = nullptr;
		QLineEdit * _fieldStepNumberEquilibrationNpt = nullptr;
		QLineEdit * _fieldStepNumberProduction		 = nullptr;

		QLineEdit * _fieldAnionName	   = nullptr;
		QLineEdit * _fieldAnionCharge  = nullptr;
		QLineEdit * _fieldCationName   = nullptr;
		QLineEdit * _fieldCationCharge = nullptr;

	  public:
		MdBasicParamForm() = default;

		void setupUi(
			QWidget *							 p_container,
			const MdParametersBasic &			 p_defaults = MdParametersBasic()
		);
		void update( const EngineSpecificCommonFormData & p_data ) noexcept;

		void get( MdParametersBasic & ) const noexcept;

		QFormLayout * layoutFieldsMdEngine() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui
#endif

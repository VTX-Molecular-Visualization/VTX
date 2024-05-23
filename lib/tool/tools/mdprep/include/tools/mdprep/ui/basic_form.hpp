#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

class QLineEdit;
class QComboBox;
class QLabel;
class QString;

namespace VTX::Tool::Mdprep::ui
{
	struct MdParametersBasic;
	struct MdAdvancedDataSample;

	// Class responsible for setting up the form for basic MD parameters
	class MdBasicParamForm
	{
	  public:
		MdBasicParamForm() = default;

		void setupUi( QWidget * p_container, const MdParametersBasic & p_defaults = MdParametersBasic() );
		void update( const EngineSpecificCommonFormData & p_data ) noexcept;
		void update( const MdAdvancedDataSample & p_data ) noexcept;

		void get( MdParametersBasic & ) const noexcept;

		QFormLayout * layoutFieldsMdEngine() noexcept;

	  private:
		QFormLayout * _layoutFieldEngineSpecific = nullptr;

		QComboBox * _fieldForceFieldBio	 = nullptr;
		QComboBox * _fieldForceFieldChem = nullptr;

		QLineEdit * _fieldStepNumberMinimization	 = nullptr;
		QLineEdit * _fieldStepNumberEquilibrationNvt = nullptr;
		QLineEdit * _fieldStepNumberEquilibrationNpt = nullptr;
		QLineEdit * _fieldStepNumberProduction		 = nullptr;

		QLineEdit * _fieldAnionName	   = nullptr;
		QComboBox * _fieldAnionCharge  = nullptr;
		QLineEdit * _fieldCationName   = nullptr;
		QComboBox * _fieldCationCharge = nullptr;

		MdAdvancedDataSample _lastSeenAdvancedDataSample;
		QLabel *			 _labelDurationEquilibrationNvt = nullptr;
		QLabel *			 _labelDurationEquilibrationNpt = nullptr;
		QLabel *			 _labelDurationProduction		= nullptr;

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadDefaultValues( const MdParametersBasic & p_defaults ) noexcept;

		void _updateEquilibrationNvt( const QString & ) noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui
#endif

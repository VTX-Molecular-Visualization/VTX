#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_ADVANCED_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_ADVANCED_FORM__

#include <functional>

class QLineEdit;
class QComboBox;
class QLabel;
class QString;
class QWidget;
class QDoubleSpinBox;

namespace VTX::Tool::Mdprep::ui
{
	struct MdParametersAdvanced;
	struct MdAdvancedDataSample;
	struct MdBasicDataSample;

	// Class Responsible for setting up the form for advanced MD parameters
	class MdAdvancedParamForm
	{
	  public:
		using UpdateCallback = std::function<void( const MdAdvancedDataSample & )>;

		MdAdvancedParamForm() = default;

		void setupUi( QWidget * p_container, const MdParametersAdvanced & p_defaults = MdParametersAdvanced() );

		void subscribe( UpdateCallback ) noexcept;
		void update( const MdBasicDataSample & p_data ) noexcept;
		void get( MdParametersAdvanced & ) const noexcept;
		void get( MdAdvancedDataSample & ) const noexcept;

	  private:
		UpdateCallback _updateCallback;

		QLineEdit * _fieldMinimizationStepSize		  = nullptr;
		QLineEdit * _fieldMinimizationStopThreshold	  = nullptr;
		QLineEdit * _fieldProductionNstxoutCompressed = nullptr;
		struct FieldSet
		{
			QLineEdit *		 nsteps		 = nullptr;
			QLabel *		 labelNsteps = nullptr;
			QDoubleSpinBox * dt			 = nullptr;
			QLineEdit *		 nstxout	 = nullptr;
			QLineEdit *		 nstvout	 = nullptr;
			QLineEdit *		 nstenergy	 = nullptr;
			QLineEdit *		 nstlog		 = nullptr;
		};
		FieldSet _equilibrationNvt;
		FieldSet _equilibrationNpt;
		FieldSet _production;

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadDefaultValues( const MdParametersAdvanced & p_defaults ) noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif

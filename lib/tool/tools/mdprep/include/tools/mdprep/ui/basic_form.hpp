#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

#include <functional>

class QLineEdit;
class QComboBox;
class QLabel;
class QString;
class QWidget;

namespace VTX::Tool::Mdprep::ui
{
	struct MdBasicDataSample;
	struct MdParametersBasic;
	struct MdAdvancedDataSample;

	// Class responsible for setting up the form for basic MD parameters
	class MdBasicParamForm
	{
	  public:
		using UpdateCallback = std::function<void( const MdBasicDataSample & )>;

		MdBasicParamForm() = default;

		void setupUi( QWidget * p_container, const MdParametersBasic & p_defaults = MdParametersBasic() );
		void update( const EngineSpecificCommonFormData & p_data ) noexcept;
		void update( const MdAdvancedDataSample & p_data ) noexcept;
		void subscribe( UpdateCallback ) noexcept;

		void get( MdParametersBasic & ) const noexcept;

		QFormLayout * layoutFieldsMdEngine() noexcept;

	  private:
		struct UiObjects
		{
			QFormLayout * _layoutFieldEngineSpecific = nullptr;

			QComboBox * _fieldForceFieldBio	 = nullptr;
			QComboBox * _fieldForceFieldChem = nullptr;

			QLineEdit * _fieldStepNumberMinimization		= nullptr;
			QLineEdit * _fieldDurationValueEquilibrationNvt = nullptr;
			QComboBox * _fieldDurationUnitEquilibrationNvt	= nullptr;
			QLineEdit * _fieldDurationValueEquilibrationNpt = nullptr;
			QComboBox * _fieldDurationUnitEquilibrationNpt	= nullptr;
			QLineEdit * _fieldDurationValueProduction		= nullptr;
			QComboBox * _fieldDurationUnitProduction		= nullptr;

			QLineEdit * _fieldAnionName	   = nullptr;
			QComboBox * _fieldAnionCharge  = nullptr;
			QLineEdit * _fieldCationName   = nullptr;
			QComboBox * _fieldCationCharge = nullptr;
		} _uiObjects;

		MdAdvancedDataSample _lastSeenAdvancedDataSample;
		UpdateCallback		 _updateCallback = []( const MdBasicDataSample & ) {};

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadValues( const MdParametersBasic & p_defaults ) noexcept;
		void _sendUpdateSignal() noexcept;

		class EventToggler
		{
		  public:
			EventToggler( std::function<void()> );

			void toggle( UiObjects & ) noexcept;

		  private:
			std::function<void()> _lambdaToConnect;
			bool				  _eventsOn = false;

			void _connect( UiObjects & ) noexcept;
			void _disconnect( UiObjects & ) noexcept;
		};
		EventToggler _eventToggler { [ & ]() { this->_sendUpdateSignal(); } };
	};
} // namespace VTX::Tool::Mdprep::ui
#endif

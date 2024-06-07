#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

#include <functional>
#include <optional>

class QLineEdit;
class QComboBox;
class QLabel;
class QString;
class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdBasicDataSample;
	struct MdAdvancedDataSample;
	struct MdParameters;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class BasicFormSettingsDialog;
	enum class E_FIELD_SECTION;
	// Class responsible for setting up the form for basic MD parameters
	class MdBasicParamForm
	{
	  public:
		using UpdateCallback			   = std::function<void( const MdBasicDataSample & )>;
		using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

		MdBasicParamForm() = default;

		void setupUi( QWidget * p_container, const MdParameters & p_defaults = MdParameters() );
		void update( const EngineSpecificCommonInformation & p_data ) noexcept;
		void update( const MdAdvancedDataSample & p_data ) noexcept;
		void subscribe( UpdateCallback ) noexcept;
		void subscribe( SpecificFieldsPlacerCallback ) noexcept;

		void get( MdParameters & ) const noexcept;

		QFormLayout * layoutFieldsMdEngine() noexcept;

	  private:
		// Data structure that put all ui entities together
		struct UiObjects
		{
			QFormLayout * _layoutFieldEngineSpecific = nullptr;

			QComboBox * _fieldForceFieldBio	 = nullptr;
			QComboBox * _fieldForceFieldChem = nullptr;

			QLineEdit *	  _fieldStepNumberMinimization		  = nullptr;
			QPushButton * _buttonMinimizationSettings		  = nullptr;
			QLineEdit *	  _fieldDurationValueEquilibrationNvt = nullptr;
			QComboBox *	  _fieldDurationUnitEquilibrationNvt  = nullptr;
			QPushButton * _buttonEquilibrationNvtSettings	  = nullptr;
			QLineEdit *	  _fieldDurationValueEquilibrationNpt = nullptr;
			QComboBox *	  _fieldDurationUnitEquilibrationNpt  = nullptr;
			QPushButton * _buttonEquilibrationNptSettings	  = nullptr;
			QLineEdit *	  _fieldDurationValueProduction		  = nullptr;
			QComboBox *	  _fieldDurationUnitProduction		  = nullptr;
			QPushButton * _buttonProductionSettings			  = nullptr;

			QLineEdit * _fieldAnionName	   = nullptr;
			QComboBox * _fieldAnionCharge  = nullptr;
			QLineEdit * _fieldCationName   = nullptr;
			QComboBox * _fieldCationCharge = nullptr;

		} _uiObjects;
		MdParameters						   _parameters;
		std::optional<BasicFormSettingsDialog> _settingsDialog;

		MdAdvancedDataSample		 _lastSeenAdvancedDataSample;
		UpdateCallback				 _updateCallback = []( const MdBasicDataSample & ) {};
		SpecificFieldsPlacerCallback _mdEngineFieldPlacerCallback;

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadValues( const MdParameters & p_defaults ) noexcept;
		void _sendUpdateSignal() noexcept;
		void _openSettingsMinimization() noexcept;
		void _openSettingsNvt() noexcept;
		void _openSettingsNpt() noexcept;
		void _openSettingsProd() noexcept;

		// Class responsible for managing callbacks between the EventToggler and the form.
		class EventCallbacks
		{
		  public:
			using Callback = std::function<void()>;
			void registerUpdate( Callback ) noexcept;
			void registerSettingPushedMinimization( Callback ) noexcept;
			void registerSettingPushedNvt( Callback ) noexcept;
			void registerSettingPushedNpt( Callback ) noexcept;
			void registerSettingPushedProd( Callback ) noexcept;

			const Callback & signalUpdate() const noexcept;
			const Callback & signalSettingPushedMinimization() const noexcept;
			const Callback & signalSettingPushedNvt() const noexcept;
			const Callback & signalSettingPushedNpt() const noexcept;
			const Callback & signalSettingPushedProd() const noexcept;

		  private:
			Callback _update;
			Callback _pushedMin;
			Callback _pushedNvt;
			Callback _pushedNpt;
			Callback _pushedProd;
		};

		// Class responsible for binding and unbinding forms callbacks
		class EventToggler
		{
		  public:
			EventToggler( const EventCallbacks & );

			void toggle( UiObjects & ) noexcept;

		  private:
			const EventCallbacks * _callbacks = nullptr;
			bool				   _eventsOn  = false;

			void _connect( UiObjects & ) noexcept;
			void _disconnect( UiObjects & ) noexcept;
		};
		EventCallbacks _eventCallbacks;
		EventToggler   _eventToggler { const_cast<const EventCallbacks &>( _eventCallbacks ) };
	}; // namespace VTX::Tool::Mdprep::ui
} // namespace VTX::Tool::Mdprep::ui
#endif

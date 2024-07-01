#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC__

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

	struct FormLayouts;
	class FormBasicSettingsDialog;
	enum class E_FIELD_SECTION;
	// Class responsible for setting up the form for basic MD parameters
	class FormBasic
	{
	  public:
		using UpdateCallback			   = std::function<void( const MdBasicDataSample & )>;
		using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

		FormBasic() = delete;
		FormBasic(
			QWidget * p_container,
			SpecificFieldsPlacerCallback,
			const MdParameters & p_defaults = MdParameters()
		);
		FormBasic( const FormBasic & )			   = delete;
		FormBasic & operator=( const FormBasic & ) = delete;
		FormBasic( FormBasic && )				   = default;
		FormBasic & operator=( FormBasic && )	   = default;
		~FormBasic();

		void update( const EngineSpecificCommonInformation & p_data ) noexcept;

		void get( MdParameters & ) const noexcept;
		void get( FormLayouts & ) const noexcept;

	  private:
		// Data structure that put all ui entities together
		struct UiData
		{
			QWidget * _parent	 = nullptr;
			QWidget * _container = nullptr;

			QFormLayout * _layoutFieldEngineSpecific = nullptr;

			QLabel *	  _labelSystemCheckMsg	= nullptr;
			QPushButton * _buttonSystemSettings = nullptr;
			QComboBox *	  _fieldForceFieldBio	= nullptr;
			QComboBox *	  _fieldForceFieldChem	= nullptr;

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

			QFormLayout * _layoutIons		 = nullptr;
			QLineEdit *	  _fieldAnionName	 = nullptr;
			QComboBox *	  _fieldAnionCharge	 = nullptr;
			QLineEdit *	  _fieldCationName	 = nullptr;
			QComboBox *	  _fieldCationCharge = nullptr;
		};
		// Used in an unique_ptr so the default move ctor invalidate the ptr that trigger deletion
		std::unique_ptr<UiData>					 _uiObjects = std::make_unique<UiData>();
		MdParameters							 _parameters;
		std::optional<FormBasicSettingsDialog>	 _settingsDialog = std::nullopt;
		Gateway::EngineSpecificCommonInformation _lastFormData;
		std::shared_ptr<bool>					 _uiShouldBeDeleted = std::make_shared<bool>( true );

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
		void _openSettingsSystem() noexcept;

		// Class responsible for managing callbacks between the EventToggler and the form.
		class EventCallbacks
		{
		  public:
			using Callback = std::function<void()>;
			void registerSettingPushedMinimization( Callback ) noexcept;
			void registerSettingPushedNvt( Callback ) noexcept;
			void registerSettingPushedNpt( Callback ) noexcept;
			void registerSettingPushedProd( Callback ) noexcept;
			void registerSettingPushedSystem( Callback ) noexcept;

			const Callback & signalSettingPushedMinimization() const noexcept;
			const Callback & signalSettingPushedNvt() const noexcept;
			const Callback & signalSettingPushedNpt() const noexcept;
			const Callback & signalSettingPushedProd() const noexcept;
			const Callback & signalSettingPushedSystem() const noexcept;

		  private:
			Callback _update;
			Callback _pushedMin;
			Callback _pushedNvt;
			Callback _pushedNpt;
			Callback _pushedProd;
			Callback _pushedSystem;
		};

		// Class responsible for binding and unbinding forms callbacks
		class EventToggler
		{
		  public:
			EventToggler( const EventCallbacks & );

			void toggle( UiData & ) noexcept;

		  private:
			const EventCallbacks * _callbacks = nullptr;
			bool				   _eventsOn  = false;

			void _connect( UiData & ) noexcept;
			void _disconnect( UiData & ) noexcept;
		};
		EventCallbacks _eventCallbacks;
		EventToggler   _eventToggler { const_cast<const EventCallbacks &>( _eventCallbacks ) };
	}; // namespace VTX::Tool::Mdprep::ui
} // namespace VTX::Tool::Mdprep::ui
#endif

#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC__

#include <functional>
#include <optional>

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QString;
class QWidget;

namespace VTX::UI::QT::Util
{
	class ObjectOwnership;
}
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
	class IonicFields;
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

		void update( const EngineSpecificCommonInformation & p_data ) noexcept;

		void get( MdParameters & ) const noexcept;
		void get( FormLayouts & ) const noexcept;

	  private:
		std::array<VTX::UI::QT::Util::ObjectOwnership, 2> _toDeleteObjects;

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
		} _uiObjects;
		IonicFields _fieldsIons;

		// Class responsible for managing event connection resource
		class SettingsButtons
		{
		  public:
			Gateway::EngineSpecificCommonInformation lastFormData;
			MdParameters							 parameters;

			SettingsButtons( MdParameters, SpecificFieldsPlacerCallback );
			SettingsButtons() = delete;
			~SettingsButtons();
			SettingsButtons( SettingsButtons && );
			SettingsButtons & operator=( SettingsButtons && );
			SettingsButtons( const SettingsButtons & )			   = delete;
			SettingsButtons & operator=( const SettingsButtons & ) = delete;

			void setMinimizationSettings( QPushButton * ) noexcept;
			void setNvtSettings( QPushButton * ) noexcept;
			void setNptSettings( QPushButton * ) noexcept;
			void setProductionSettings( QPushButton * ) noexcept;
			void setSystemSettings( QPushButton * ) noexcept;

		  private:
			SpecificFieldsPlacerCallback		   _mdEngineFieldPlacerCallback;
			std::optional<FormBasicSettingsDialog> _settingsDialog = std::nullopt;
			struct UiObjects
			{
				QPushButton * _buttonMinimizationSettings	  = nullptr;
				QPushButton * _buttonEquilibrationNvtSettings = nullptr;
				QPushButton * _buttonEquilibrationNptSettings = nullptr;
				QPushButton * _buttonProductionSettings		  = nullptr;
				QPushButton * _buttonSystemSettings			  = nullptr;
			} _uiObjects;
			static void _disconnectAll( UiObjects & ) noexcept;

			void _connectSettingsMinimization() noexcept;
			void _connectSettingsNvt() noexcept;
			void _connectSettingsNpt() noexcept;
			void _connectSettingsProd() noexcept;
			void _connectSettingsSystem() noexcept;
			void _openSettingsMinimization() noexcept;
			void _openSettingsNvt() noexcept;
			void _openSettingsNpt() noexcept;
			void _openSettingsProd() noexcept;
			void _openSettingsSystem() noexcept;
		};
		SettingsButtons _settingsButtons;
		void			_createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void			_loadValues( const MdParameters & p_defaults ) noexcept;
		void			_setupSlots() noexcept;

	}; // namespace VTX::Tool::Mdprep::ui
} // namespace VTX::Tool::Mdprep::ui
#endif

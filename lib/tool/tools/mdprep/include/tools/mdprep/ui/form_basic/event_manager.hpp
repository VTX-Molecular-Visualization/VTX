#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_EVENT_MANAGER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_EVENT_MANAGER__

#include <functional>
#include <optional>
class QPushButton;
class QVBoxLayout;

namespace VTX::UI::QT::Util
{
	class LabelWithHelper;
	class ObjectOwnership;
} // namespace VTX::UI::QT::Util
namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	class InputChecker;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;
} // namespace VTX::Tool::Mdprep::ui
namespace VTX::Tool::Mdprep::ui::form_basic
{

	class Data;
	class SettingsDialog;
	using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

	using namespace VTX::Tool::Mdprep::Gateway;

	// Class responsible for managing event connection resource
	class EventManager
	{
	  public:

		EventManager( Data & ) noexcept;
		EventManager() = delete;
		~EventManager();
		EventManager( EventManager && ) noexcept;
		EventManager & operator=( EventManager && ) noexcept;
		EventManager( const EventManager & )			 = delete;
		EventManager & operator=( const EventManager & ) = delete;

		void setMinimizationSettings( QPushButton * ) noexcept;
		void setNvtSettings( QPushButton * ) noexcept;
		void setNptSettings( QPushButton * ) noexcept;
		void setProductionSettings( QPushButton * ) noexcept;
		void setSystemSettings( QPushButton * ) noexcept;
		void setSystemMsg( QVBoxLayout * ) noexcept;
		void startInputCheck() noexcept;

	  private:
		Data *						  _data;
		std::optional<SettingsDialog> _settingsDialog = std::nullopt;
		VTX::Util::SentryTarget		  _sentry;
		struct UiObjects
		{
			QPushButton *					   _buttonMinimizationSettings	   = nullptr;
			QPushButton *					   _buttonEquilibrationNvtSettings = nullptr;
			QPushButton *					   _buttonEquilibrationNptSettings = nullptr;
			QPushButton *					   _buttonProductionSettings	   = nullptr;
			QPushButton *					   _buttonSystemSettings		   = nullptr;
			QVBoxLayout *					   _layoutSystemCheckMsg		   = nullptr;
			VTX::UI::QT::Util::LabelWithHelper _labelSystemCheck;
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

		std::function<void( Gateway::CheckReport )> _getSystemCallback() noexcept;

		void _systemSettingsApplied() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic

#endif

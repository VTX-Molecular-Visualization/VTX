#ifndef VTX_TOOL_MDPREP_UI_FORM_BASIC_EVENT_MANAGER_HPP
#define VTX_TOOL_MDPREP_UI_FORM_BASIC_EVENT_MANAGER_HPP

#include <functional>
#include <optional>
#include <util/sentry.hpp>
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
	enum class E_FIELD_SECTION : uint8_t;
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

		void setMinimizationSettings( QPushButton * );
		void setInputCheck( QPushButton * ) noexcept;
		void setNvtSettings( QPushButton * );
		void setNptSettings( QPushButton * );
		void setProductionSettings( QPushButton * );
		void setSystemSettings( QPushButton * );
		void setSystemMsg( QVBoxLayout * );
		void performFirstInputCheck();

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
			QPushButton *					   _buttonInputCheck			   = nullptr;
			VTX::UI::QT::Util::LabelWithHelper _labelSystemCheck;
		} _uiObjects;

		static void _disconnectAll( UiObjects & ) noexcept;

		void _connectSettingsMinimization();
		void _connectSettingsNvt();
		void _connectSettingsNpt();
		void _connectSettingsProd();
		void _connectSettingsManager();
		void _connectInputCheck();
		void _openSettingsMinimization();
		void _openSettingsNvt();
		void _openSettingsNpt();
		void _openSettingsProd();
		void _openSettingsManager();
		void _startInputCheck();

		void _systemSettingsApplied() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic

#endif

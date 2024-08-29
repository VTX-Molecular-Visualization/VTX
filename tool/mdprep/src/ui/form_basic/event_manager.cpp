#include <qboxlayout.h>
#include <qpushbutton.h>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_basic/data.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <app/vtx_app.hpp>
#include <qt/util.hpp>
#include <util/sentry.hpp>
//
#include "tool/mdprep/ui/form_basic/settings_dialog.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_basic/event_manager.hpp"

namespace VTX::Tool::Mdprep::ui::form_basic
{
	EventManager::EventManager( Data & p_ ) noexcept : _data( &p_ ) {}

	EventManager::~EventManager() { _disconnectAll( _uiObjects ); }

	EventManager::EventManager( EventManager && p_ ) noexcept :
		_data( p_._data ), _settingsDialog( std::move( p_._settingsDialog ) ), _sentry( std::move( p_._sentry ) )
	{
		_disconnectAll( p_._uiObjects );
		std::swap( p_._uiObjects, _uiObjects );
		_connectSettingsMinimization();
		_connectSettingsNvt();
		_connectSettingsNpt();
		_connectSettingsProd();
		_connectSettingsSystem();
		_connectInputCheck();
	}

	EventManager & EventManager::operator=( EventManager && p_ ) noexcept
	{
		if ( &p_ == this )
			return *this;

		_data			= p_._data;
		_settingsDialog = std::move( p_._settingsDialog );
		_sentry			= std::move( p_._sentry );
		_disconnectAll( _uiObjects );
		_disconnectAll( p_._uiObjects );
		_uiObjects = std::move( p_._uiObjects );

		p_._uiObjects = UiObjects();
		_connectSettingsMinimization();
		_connectSettingsNvt();
		_connectSettingsNpt();
		_connectSettingsProd();
		_connectSettingsSystem();
		_connectInputCheck();

		return *this;
	}

	void EventManager::setMinimizationSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonMinimizationSettings = p_;
		_connectSettingsMinimization();
	}

	void EventManager::setInputCheck( QPushButton * p_ ) noexcept { _uiObjects._buttonInputCheck = p_; }

	void EventManager::setNvtSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonEquilibrationNvtSettings = p_;
		_connectSettingsNvt();
	}

	void EventManager::setNptSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonEquilibrationNptSettings = p_;
		_connectSettingsNpt();
	}

	void EventManager::setProductionSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonProductionSettings = p_;
		_connectSettingsProd();
	}

	void EventManager::setSystemSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonSystemSettings = p_;
		_connectSettingsSystem();
	}

	void EventManager::setSystemMsg( QVBoxLayout * p_ ) noexcept
	{
		_uiObjects._layoutSystemCheckMsg = p_;
		_relocateCheckReport();
	}

	void EventManager::_disconnectAll( UiObjects & p_uiObjects ) noexcept
	{
		if ( p_uiObjects._buttonMinimizationSettings )
			p_uiObjects._buttonMinimizationSettings->disconnect( p_uiObjects._buttonMinimizationSettings );
		if ( p_uiObjects._buttonEquilibrationNvtSettings )
			p_uiObjects._buttonEquilibrationNvtSettings->disconnect( p_uiObjects._buttonEquilibrationNvtSettings );
		if ( p_uiObjects._buttonEquilibrationNptSettings )
			p_uiObjects._buttonEquilibrationNptSettings->disconnect( p_uiObjects._buttonEquilibrationNptSettings );
		if ( p_uiObjects._buttonProductionSettings )
			p_uiObjects._buttonProductionSettings->disconnect( p_uiObjects._buttonProductionSettings );
		if ( p_uiObjects._buttonSystemSettings )
			p_uiObjects._buttonSystemSettings->disconnect( p_uiObjects._buttonSystemSettings );
	}
	void EventManager::_connectSettingsMinimization() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonMinimizationSettings,
			&QPushButton::clicked,
			[ & ]() { this->_openSettingsMinimization(); }
		);
	}
	void EventManager::_connectSettingsNvt() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonEquilibrationNvtSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsNvt(); }
		);
	}
	void EventManager::_connectSettingsNpt() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonEquilibrationNptSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsNpt(); }
		);
	}
	void EventManager::_connectSettingsProd() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonProductionSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsProd(); }
		);
	}
	void EventManager::_connectSettingsSystem() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonSystemSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsSystem(); }
		);
	}
	void EventManager::_connectInputCheck() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonInputCheck, &QPushButton::clicked, [ & ]() { this->_startInputCheck(); }
		);
	}
	void EventManager::_openSettingsMinimization() noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		SpecificFieldsPlacerCallback * callback = nullptr;
		_data->get( callback );
		_settingsDialog.emplace(
			_uiObjects._buttonMinimizationSettings,
			"Minimization Settings",
			&params->minimization,
			( *callback )( E_FIELD_SECTION::minimization )
		);
	}
	void EventManager::_openSettingsNvt() noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		SpecificFieldsPlacerCallback * callback = nullptr;
		_data->get( callback );
		_settingsDialog.emplace(
			_uiObjects._buttonEquilibrationNvtSettings,
			"Nvt Equilibration Settings",
			&params->nvt,
			( *callback )( E_FIELD_SECTION::equilibrationNvt )
		);
	}
	void EventManager::_openSettingsNpt() noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		SpecificFieldsPlacerCallback * callback = nullptr;
		_data->get( callback );
		_settingsDialog.emplace(
			_uiObjects._buttonEquilibrationNptSettings,
			"Npt Equilibration Settings",
			&params->npt,
			( *callback )( E_FIELD_SECTION::equilibrationNpt )
		);
	}
	void EventManager::_openSettingsProd() noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		SpecificFieldsPlacerCallback * callback = nullptr;
		_data->get( callback );
		_settingsDialog.emplace(
			_uiObjects._buttonProductionSettings,
			"Production run Settings",
			&params->prod,
			( *callback )( E_FIELD_SECTION::production )
		);
	}
	namespace
	{
		class ApplyVisitorBasic
		{
		  public:
			ApplyVisitorBasic( std::function<void()> p_callback ) : _callback( std::move( p_callback ) ) {}
			void applied() noexcept { _callback(); }

		  private:
			std::function<void()> _callback;
		};
	} // namespace
	void EventManager::_openSettingsSystem() noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		SpecificFieldsPlacerCallback * callback = nullptr;
		_data->get( callback );
		Gateway::EngineSpecificCommonInformation * info = nullptr;
		_data->get( info );
		_settingsDialog.emplace(
			_uiObjects._buttonSystemSettings,
			"System Settings",
			&params->system,
			( *callback )( E_FIELD_SECTION::system ),
			*info,
			ApplyVisitorBasic( [ & ]() { this->_startInputCheck(); } )
		);
	}
	void EventManager::performFirstInputCheck() noexcept
	{
		ReportManager * reportManager = nullptr;
		_data->get( reportManager );
		if ( reportManager->hasFirstCheckBeenDone() )
			return;

		_startInputCheck();
	}
	void EventManager::_startInputCheck() noexcept
	{
		ReportManager * reportManager = nullptr;
		_data->get( reportManager );

		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		reportManager->checkInputs( *params );
	}
	void EventManager::_relocateCheckReport() noexcept
	{
		ReportManager * reportManager = nullptr;
		_data->get( reportManager );
		reportManager->resetReportLocation( _uiObjects._layoutSystemCheckMsg, _sentry );
	}
	void EventManager::_systemSettingsApplied() noexcept {}
} // namespace VTX::Tool::Mdprep::ui::form_basic

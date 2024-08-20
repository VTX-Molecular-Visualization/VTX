#include <qboxlayout.h>
#include <qpushbutton.h>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/form_basic/data.hpp"
#include "tools/mdprep/ui/shared.hpp"
#include <app/vtx_app.hpp>
#include <ui/qt/util.hpp>
#include <util/sentry.hpp>
//
#include "tools/mdprep/ui/form_basic/settings_dialog.hpp"
//
#include "tools/mdprep/ui/form_basic/event_manager.hpp"

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

		return *this;
	}

	void EventManager::setMinimizationSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonMinimizationSettings = p_;
		_connectSettingsMinimization();
	}

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

	void EventManager::setSystemMsg( QVBoxLayout * p_ ) noexcept { _uiObjects._layoutSystemCheckMsg = p_; }

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
			ApplyVisitorBasic( [ & ]() { this->startInputCheck(); } )
		);
	}
	void EventManager::startInputCheck() noexcept
	{
		if ( _uiObjects._layoutSystemCheckMsg == nullptr )
			return;
		if ( _uiObjects._labelSystemCheck.container )
			delete _uiObjects._labelSystemCheck.container;
		_uiObjects._labelSystemCheck = VTX::UI::QT::Util::LabelWithHelper {
			"Checking System-forcefield compatibility ...",
			"VTX is currently checking if the selected MD Engine support your system with the selected forcefield.",
			VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
		};
		_uiObjects._layoutSystemCheckMsg->addWidget( _uiObjects._labelSystemCheck );
		InputChecker * inputChecker = nullptr;
		_data->get( inputChecker );

		if ( inputChecker->isResultAvailable() )
		{
			_getSystemCallback()( inputChecker->lastResult() );
			return;
		}

		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		inputChecker->checkInputs( *params, _getSystemCallback() );
	}
	namespace
	{

		class ReportResultPoster
		{
		  public:
			ReportResultPoster(
				Gateway::CheckReport				 p_report,
				QVBoxLayout *						 p_target,
				VTX::UI::QT::Util::LabelWithHelper & p_label,
				VTX::Util::Sentry					 p_sentry
			) :
				_report( std::move( p_report ) ), _target( p_target ), _label( &p_label ),
				_sentry( std::move( p_sentry ) )
			{
			}

			void operator()() noexcept
			{
				if ( not _sentry )
					return;

				if ( _label->container )
					delete _label->container;
				*_label = VTX::UI::QT::Util::LabelWithHelper(
					getReportLabel( _report.pass ),
					_report.message.c_str(),
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				);
				_target->addWidget( *_label );
			}

		  private:
			Gateway::CheckReport				 _report;
			QVBoxLayout *						 _target;
			VTX::UI::QT::Util::LabelWithHelper * _label;
			VTX::Util::Sentry					 _sentry;
		};

		class ReportResultWaiter
		{
		  public:
			ReportResultWaiter(
				VTX::Util::Sentry					 p_sentry,
				QVBoxLayout *						 p_target,
				VTX::UI::QT::Util::LabelWithHelper & p_label
			) : _target( p_target ), _sentry( std::move( p_sentry ) ), _label( &p_label )
			{
			}

			void operator()( Gateway::CheckReport p_report ) noexcept
			{
				if ( p_report.itemGeneric != E_REPORT_CHECKED_ITEM::systemWithForceField )
					return;

				VTX::App::VTXApp::get().onEndOfFrameOneShot
					+= ReportResultPoster( std::move( p_report ), _target, *_label, _sentry );
			}

		  private:
			QVBoxLayout *						 _target = nullptr;
			VTX::Util::Sentry					 _sentry;
			VTX::UI::QT::Util::LabelWithHelper * _label = nullptr;
		};
	} // namespace
	std::function<void( Gateway::CheckReport )> EventManager::_getSystemCallback() noexcept
	{
		return ReportResultWaiter(
			_sentry.newSentry(), _uiObjects._layoutSystemCheckMsg, _uiObjects._labelSystemCheck
		);
	}
	void EventManager::_systemSettingsApplied() noexcept {}
} // namespace VTX::Tool::Mdprep::ui::form_basic

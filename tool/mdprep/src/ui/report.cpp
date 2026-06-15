#include <latch>
//
#include <QPointer>
#include <QTimer>
//
#include <util/event_hub.hpp>
#include <util/sentry.hpp>
//
#include <app/action/action_manager.hpp>
#include <app/services.hpp>
#include <app/threading/base_thread.hpp>
#include <ui/qt/util.hpp>
//
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <tool/mdprep/gateway/form_data.hpp>
//
#include "tool/mdprep/ui/report.hpp"

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{

		inline VTX::UI::QT::Util::LabelWithHelper getWaitingMessage()
		{
			return VTX::UI::QT::Util::LabelWithHelper {
				"Checking System-forcefield compatibility "
				"...",
				"VTX is currently checking if the selected MD Engine suppo<a "
				"href='https://youtu.be/dQw4w9WgXcQ?si=TeMDxgYrLTIscnso'>r</a>t your system with the selected "
				"forcefield.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			};
		}

		inline void createReportUi( ReportUi & p_reportUi )
		{
			if ( p_reportUi.report == Gateway::CheckReport() )
			{
				p_reportUi.content = getWaitingMessage();
				return;
			}
			p_reportUi.content = VTX::UI::QT::Util::LabelWithHelper(
				getReportLabel( p_reportUi.report.pass ),
				p_reportUi.report.message.c_str(),
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);

			p_reportUi.content.label->setWordWrap( true );
		}

		class ReportResultPoster
		{
		  public:
			ReportResultPoster( ReportManager::Data & p_reportData, UiReportCallback p_sendReportUi ) :
				_reportData( &p_reportData ), _sendReportUi( std::move( p_sendReportUi ) )
			{
			}

			void operator()() noexcept
			{
				ReportUi reportUi { .report = _reportData->report };
				createReportUi( reportUi );
				_sendReportUi( reportUi );
				_reportData->checkInProgress = false;
			}

		  private:
			ReportManager::Data * _reportData = nullptr;
			UiReportCallback	  _sendReportUi;
		};

		class ReportResultWaiter
		{
		  public:
			ReportResultWaiter( ReportManager::Data & p_reportData, UiReportCallback p_sendReportUi ) :
				_reportData( &p_reportData ), _sendReportUi( std::move( p_sendReportUi ) )
			{
			}

			void operator()( Gateway::CheckReport p_report ) noexcept
			{
				if ( p_report.itemGeneric != Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField )
				{
					return;
				}
				_reportData->report = p_report;
				QTimer::singleShot( 0, [ & ]() { ReportResultPoster( *_reportData, std::move( _sendReportUi ) ); } );
			}

		  private:
			ReportManager::Data * _reportData = nullptr;
			UiReportCallback	  _sendReportUi;
		};
	} // namespace

	ReportManager::ReportManager( InputChecker p_inputChecker ) : _inputChecker( std::move( p_inputChecker ) ) {

	}

	bool ReportManager::hasFirstCheckBeenDone() const noexcept { return firstCheckStarted; }

	namespace
	{
		struct TriggerReportDisplay
		{
			void execute( const Mdprep::Gateway::CheckReport & p_report )
			{ App::HUB().trigger<Mdprep::Gateway::CheckReport>( p_report ); }
		};

		struct CheckInputsAction
		{
			App::Threading::ThreadData thrData;
			std::latch				   waiter { 1 };

			CheckInputsAction() = default;

			CheckInputsAction( App::Threading::ThreadData p_ ) : thrData( std::move( p_ ) ) {}

			void wait() noexcept { waiter.wait(); }

			void execute( std::reference_wrapper<InputChecker> p_inputChecker, Gateway::MdParameters p_params )
			{
				p_inputChecker.get().checkInputs( p_params, thrData );
				App::ACTION().subscribe(
					App::Action::QueuedAction( TriggerReportDisplay(), p_inputChecker.get().lastResult() )
				);

				waiter.count_down();
			}
		};
	} // namespace

	void ReportManager::checkInputs( const Gateway::MdParameters & p_params ) noexcept
	{
		if ( _reportData.checkInProgress )
		{
			return;
		}

		firstCheckStarted			= true;
		_reportData.report			= Gateway::CheckReport();
		_reportData.checkInProgress = true;

		auto callback = _uiManager.produceCallback();
		callback( { getWaitingMessage() } );
		App::ACTION().execute<CheckInputsAction>( std::reference_wrapper<InputChecker>( _inputChecker ), p_params );
	}

	void ReportManager::relocate( QPointer<QVBoxLayout> p_ ) noexcept { _uiManager.relocate( std::move( p_ ) ); }

	void ReportManager::relocate( ReportManager & p_ ) noexcept { _uiManager.relocate( p_._uiManager ); }

	class FramedReportManager
	{
	  public:
		void relocate( FramedReportManager & p_other ) noexcept
		{
			_removeContainerFromLayout();
			_deleteContainer();
			_deleteLayout();

			_currentLayout = p_other._currentLayout;

			p_other._removeContainerFromLayout();
			p_other._deleteContainer();
			p_other._currentLayout = nullptr;

			if ( not _currentLayout.isNull() )
			{
				_createContainer();
			}

			createReportUi( _lastUiReport );
			_recreateUi();
		}

		void relocate( QPointer<QVBoxLayout> p_ ) noexcept
		{
			_removeContainerFromLayout();
			_deleteContainer();
			_currentLayout = p_;

			if ( not _lastUiReport.content.container.isNull() )
			{
				delete _lastUiReport.content.container;
			}

			if ( _currentLayout.isNull() )
			{
				return;
			}

			createReportUi( _lastUiReport );
			_recreateUi();
		}

		void postReport( ReportUi p_uiReport ) noexcept
		{
			if ( not _lastUiReport.content.container.isNull() )
			{
				delete _lastUiReport.content.container;
			}

			_lastUiReport = std::move( p_uiReport );

			_recreateUi();
		}

	  private:
		QPointer<QVBoxLayout> _currentLayout = nullptr;
		QPointer<QWidget>	  _container	 = nullptr;
		ReportUi			  _lastUiReport;

		void _recreateUi() noexcept
		{
			if ( _currentLayout.isNull() )
			{
				return;
			}
			_createContainer();

			if ( not _lastUiReport.content.container.isNull() )
			{
				_container->layout()->addWidget( _lastUiReport.content );
			}
		}

		inline void _removeContainerFromLayout() noexcept
		{
			if ( not _currentLayout.isNull() && not _container.isNull() )
			{
				_currentLayout->removeWidget( _container );
			}
		}

		inline void _deleteLayout() noexcept
		{
			if ( not _currentLayout.isNull() )
			{
				delete _currentLayout.data();
			}
		}

		inline void _deleteContainer() noexcept
		{
			if ( not _container.isNull() )
			{
				delete _container.data();
			}
		}

		inline void _createContainer() noexcept
		{
			if ( _container.isNull() )
			{
				_container = new QWidget;
				_currentLayout->addWidget( _container );
				_container->setLayout( new QVBoxLayout );
			}
		}
	};

	/**
	 * @brief Class connected to the ReportPlacer. Aims to be called to forward ui report to the UiReportManager
	 */
	class ReportPlacerCaller
	{
	  public:
		ReportPlacerCaller() = delete;

		ReportPlacerCaller( std::shared_ptr<FramedReportManager> p_in ) : _manager( std::move( p_in ) ) {}

		void operator()( ReportUi p_report ) noexcept { _manager->postReport( std::move( p_report ) ); }

	  private:
		std::shared_ptr<FramedReportManager> _manager;
	};

	struct FramedReportManagerDeleter
	{
		void operator()( FramedReportManager * p_ ) noexcept { delete p_; }
	};

	UiReportManager::UiReportManager() : _manager( new FramedReportManager, FramedReportManagerDeleter() ) {}

	void UiReportManager::relocate( QPointer<QVBoxLayout> p_layout ) noexcept
	{ _manager->relocate( std::move( p_layout ) ); }

	void UiReportManager::relocate( UiReportManager & p_other ) noexcept { _manager->relocate( *p_other._manager ); }

	UiReportCallback UiReportManager::produceCallback() noexcept { return ReportPlacerCaller( _manager ); }

} // namespace VTX::Tool::Mdprep::ui

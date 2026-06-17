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
#include <app/system/visibility.hpp>
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

		struct Data
		{
			Gateway::CheckReport report;
			bool				 checkInProgress = false;
		};

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

		struct ReportUi
		{
			ReportUi() = default;

			ReportUi( Gateway::CheckReport p_ ) : report( std::move( p_ ) )
			{
				if ( report == Gateway::CheckReport() )
				{
					content = getWaitingMessage();
					return;
				};
				content = VTX::UI::QT::Util::LabelWithHelper(
					getReportLabel( report ),
					report.message.c_str(),
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				);

				content.label->setWordWrap( true );
			}

			VTX::UI::QT::Util::LabelWithHelper content;
			Gateway::CheckReport			   report;
		};

	} // namespace

	/**
	 * @brief Internal class used for UiReport related things
	 */
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
			_lastUiReport = ReportUi( _lastUiReport.report );
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

			_lastUiReport = ReportUi( _lastUiReport.report );
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

	struct ReportManager::_Impl
	{
		InputChecker					 _inputChecker;
		Util::EventHub::ScopedConnection _visibilityChanged {
			App::REG().on_update<App::System::Visibility>().connect<&ReportManager::_Impl::visibilityChanged>( this )
		};
		Util::EventHub::ScopedConnection _reportReception {
			App::HUB().connect<Gateway::CheckReport, &ReportManager::_Impl::_receiveReport>( this )
		};
		FramedReportManager _manager;
		Data				_reportData;
		bool				firstCheckStarted = false;

		inline bool hasFirstCheckBeenDone() const noexcept { return firstCheckStarted; }

		inline void checkInputs( const Gateway::MdParameters & p_params ) noexcept
		{
			if ( _reportData.checkInProgress )
			{
				return;
			}

			firstCheckStarted  = true;
			_reportData.report = Gateway::CheckReport();
			_manager.postReport( ReportUi( _reportData.report ) );
			_reportData.checkInProgress = true;

			_inputChecker.checkInputs( p_params );
		}

		inline void relocate( QPointer<QVBoxLayout> p_ ) noexcept { _manager.relocate( p_ ); }

		inline void relocate( ReportManager & p_ ) noexcept { _manager.relocate( p_._impl->_manager ); }

		inline void _receiveReport( Gateway::CheckReport p_report ) noexcept
		{
			_reportData.checkInProgress = false;
			_reportData.report			= p_report;
			_manager.postReport( ReportUi( std::move( p_report ) ) );
		}

		inline void visibilityChanged( Entity ) noexcept
		{
			_reportData.report.dirty = true;
			QTimer::singleShot( 0, nullptr, [ & ] { _manager.postReport( ReportUi( _reportData.report ) ); } );
		}
	};

	ReportManager::ReportManager( InputChecker p_inputChecker ) : _impl( new _Impl( std::move( p_inputChecker ) ) ) {}

	bool ReportManager::hasFirstCheckBeenDone() const noexcept { return _impl->hasFirstCheckBeenDone(); }

	void ReportManager::checkInputs( const Gateway::MdParameters & p_params ) noexcept
	{
		assert( _impl );
		_impl->checkInputs( p_params );
	}

	void ReportManager::relocate( QPointer<QVBoxLayout> p_ ) noexcept
	{
		assert( _impl );
		_impl->relocate( std::move( p_ ) );
	}

	void ReportManager::relocate( ReportManager & p_ ) noexcept
	{
		assert( _impl );
		_impl->relocate( p_ );
	}

	void ReportManager::_receiveReport( Gateway::CheckReport p_report ) noexcept
	{
		assert( _impl );
		_impl->_receiveReport( std::move( p_report ) );
	}

} // namespace VTX::Tool::Mdprep::ui

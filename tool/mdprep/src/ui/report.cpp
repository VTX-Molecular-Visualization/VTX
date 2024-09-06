#include <QPointer>
//
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <app/vtx_app.hpp>
#include <ui/qt/util.hpp>
#include <util/sentry.hpp>
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
		inline void createReportUi( VTX::UI::QT::Util::LabelWithHelper & p_label, Gateway::CheckReport & p_report )
		{
			if ( p_report == Gateway::CheckReport() )
			{
				p_label = getWaitingMessage();
				return;
			}
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				getReportLabel( p_report.pass ),
				p_report.message.c_str(),
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);

			p_label.label->setWordWrap( true );
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
				ReportUi reportUi;
				createReportUi( reportUi.content, _reportData->report );
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
					return;
				_reportData->report = p_report;
				VTX::APP::onEndOfFrameOneShot += ReportResultPoster( *_reportData, std::move( _sendReportUi ) );
			}

		  private:
			ReportManager::Data * _reportData = nullptr;
			UiReportCallback	  _sendReportUi;
		};
	} // namespace

	ReportManager::ReportManager( InputChecker p_inputChecker ) : _inputChecker( std::move( p_inputChecker ) ) {}
	bool ReportManager::hasFirstCheckBeenDone() const noexcept { return firstCheckStarted; }
	void ReportManager::checkInputs(
		const Gateway::MdParameters & p_params,
		UiReportCallback			  p_reportCallback
	) noexcept
	{
		if ( _reportData.checkInProgress )
			return;

		firstCheckStarted			= true;
		_reportData.report			= Gateway::CheckReport();
		_reportData.checkInProgress = true;

		p_reportCallback( { getWaitingMessage() } );

		_inputChecker.checkInputs( p_params, ReportResultWaiter( _reportData, std::move( p_reportCallback ) ) );
	}

	class FramedReportManager
	{
	  public:
		void relocate( QPointer<QVBoxLayout> p_ ) noexcept { _currentLayout = p_; }
		void postReport( ReportUi p_uiReport ) noexcept
		{
			if ( _currentLayout.isNull() )
				return;
			if ( _container.isNull() )
			{
				_container = new QWidget;
				_currentLayout->addWidget( _container );
			}

			if ( not _lastUiReport.content.container.isNull() )
				delete _lastUiReport.content.container.data();

			_container->setLayout( new QVBoxLayout );
			_container->layout()->addWidget( p_uiReport.content );
			_lastUiReport = std::move( p_uiReport );
		}

	  private:
		QPointer<QVBoxLayout> _currentLayout = nullptr;
		QPointer<QWidget>	  _container	 = nullptr;
		ReportUi			  _lastUiReport;
	};

	/**
	 * @brief Class connected to the ReportPlacer. Aims to be called to forward ui report to the UiReportManager
	 */
	class ReportPlacerCaller
	{
	  public:
		ReportPlacerCaller() = delete;
		ReportPlacerCaller( std::shared_ptr<FramedReportManager> p_in ) : _manager( std::move( p_in ) ) {}
		void operator()( ReportUi p_report ) noexcept
		{
			//
			_manager->postReport( std::move( p_report ) );
			//
		}

	  private:
		std::shared_ptr<FramedReportManager> _manager;
	};

	struct FramedReportManagerDeleter
	{
		void operator()( FramedReportManager * p_ ) noexcept { delete p_; }
	};
	UiReportManager::UiReportManager() : _manager( new FramedReportManager, FramedReportManagerDeleter() ) {}

	void UiReportManager::relocate( QPointer<QVBoxLayout> p_layout ) noexcept
	{
		_manager->relocate( std::move( p_layout ) );
	}

	UiReportCallback UiReportManager::produceCallback() noexcept { return ReportPlacerCaller( _manager ); }

} // namespace VTX::Tool::Mdprep::ui

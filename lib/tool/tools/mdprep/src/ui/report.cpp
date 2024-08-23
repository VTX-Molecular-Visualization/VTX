
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/shared.hpp"
#include <app/vtx_app.hpp>
#include <ui/qt/util.hpp>
#include <util/sentry.hpp>
//
#include "tools/mdprep/ui/report.hpp"

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{

		inline VTX::UI::QT::Util::LabelWithHelper getWaitingMessage()
		{
			return VTX::UI::QT::Util::LabelWithHelper {
				"Checking System-forcefield compatibility ...",
				"VTX is currently checking if the selected MD Engine support your system with the selected forcefield.",
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
		}

		class ReportResultPoster
		{
		  public:
			ReportResultPoster( ReportManager::Data & p_reportData ) : _reportData( &p_reportData ) {}

			void operator()() noexcept
			{
				if ( _reportData->label.container )
					delete _reportData->label.container;
				createReportUi( _reportData->label, _reportData->report );
				( _reportData->target )->addWidget( _reportData->label );
				_reportData->checkInProgress = false;
			}

		  private:
			ReportManager::Data * _reportData = nullptr;
		};

		class ReportResultWaiter
		{
		  public:
			ReportResultWaiter( ReportManager::Data & p_reportData ) : _reportData( &p_reportData ) {}

			void operator()( Gateway::CheckReport p_report ) noexcept
			{
				if ( p_report.itemGeneric != Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField )
					return;
				_reportData->report = p_report;
				VTX::App::VTXApp::get().onEndOfFrameOneShot += ReportResultPoster( *_reportData );
			}

		  private:
			ReportManager::Data * _reportData = nullptr;
		};
	} // namespace

	ReportManager::ReportManager( InputChecker p_inputChecker ) : _inputChecker( std::move( p_inputChecker ) ) {}
	void ReportManager::checkInputs( const Gateway::MdParameters & p_inputs ) noexcept
	{
		if ( _reportData.checkInProgress )
			return;
		if ( _sentryTarget == nullptr && _reportData.target == nullptr )
			return;
		firstCheckStarted = true;

		if ( _reportData.label.container != nullptr )
			delete _reportData.label.container;

		_reportData.report			= Gateway::CheckReport();
		_reportData.checkInProgress = true;

		_reportData.label = getWaitingMessage();
		if ( _reportData.target )
			_reportData.target->addWidget( _reportData.label );

		_inputChecker.checkInputs( p_inputs, ReportResultWaiter( _reportData ) );
	}
	bool ReportManager::hasFirstCheckBeenDone() const noexcept { return firstCheckStarted; }
	void ReportManager::resetReportLocation(
		QVBoxLayout *			  p_newLocation,
		VTX::Util::SentryTarget & p_newLocationSentryTarget
	) noexcept
	{
		_reportData.target = p_newLocation;
		_sentryTarget	   = &p_newLocationSentryTarget;

		createReportUi( _reportData.label, _reportData.report );
		_reportData.target->addWidget( _reportData.label );
	}
} // namespace VTX::Tool::Mdprep::ui

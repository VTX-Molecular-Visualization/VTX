
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/shared.hpp"
#include <app/vtx_app.hpp>
#include <util/sentry.hpp>
//
#include "tools/mdprep/ui/report.hpp"

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{
		inline void createReportUi( VTX::UI::QT::Util::LabelWithHelper & p_label, Gateway::CheckReport & p_report )
		{
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				getReportLabel( p_report.pass ),
				p_report.message.c_str(),
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
		}

		class ReportResultPoster
		{
		  public:
			ReportResultPoster(
				Gateway::CheckReport &				 p_report,
				QVBoxLayout *						 p_target,
				VTX::UI::QT::Util::LabelWithHelper & p_label,
				VTX::Util::Sentry					 p_sentry
			) : _report( &p_report ), _target( p_target ), _label( &p_label ), _sentry( std::move( p_sentry ) )
			{
			}

			void operator()() noexcept
			{
				if ( not _sentry )
					return;

				if ( _label->container )
					delete _label->container;
				createReportUi( *_label, *_report );
				_target->addWidget( *_label );
			}

		  private:
			Gateway::CheckReport *				 _report = nullptr;
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
				Gateway::CheckReport &				 p_reportToFill,
				VTX::UI::QT::Util::LabelWithHelper & p_label
			) : _target( p_target ), _sentry( std::move( p_sentry ) ), _label( &p_label ), _report( &p_reportToFill )
			{
			}

			void operator()( Gateway::CheckReport p_report ) noexcept
			{
				if ( p_report.itemGeneric != Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField )
					return;
				*_report = p_report;
				VTX::App::VTXApp::get().onEndOfFrameOneShot
					+= ReportResultPoster( *_report, _target, *_label, _sentry );
			}

		  private:
			Gateway::CheckReport *				 _report = nullptr;
			QVBoxLayout *						 _target = nullptr;
			VTX::Util::Sentry					 _sentry;
			VTX::UI::QT::Util::LabelWithHelper * _label = nullptr;
		};
	} // namespace

	ReportManager::ReportManager( InputChecker p_inputChecker ) : _inputChecker( std::move( p_inputChecker ) ) {}
	void ReportManager::checkInputs( const Gateway::MdParameters & p_inputs ) noexcept
	{
		if ( _sentryTarget == nullptr && _reportLocation == nullptr )
			return;

		_inputChecker.checkInputs(
			p_inputs, ReportResultWaiter( _sentryTarget->newSentry(), _reportLocation, _lastReport, _label )
		);
	}
	void ReportManager::resetReportLocation(
		QVBoxLayout *			  p_newLocation,
		VTX::Util::SentryTarget & p_newLocationSentryTarget
	) noexcept
	{
		if ( _label.container != nullptr )
			delete _label.container;
		_reportLocation = p_newLocation;
		_sentryTarget	= &p_newLocationSentryTarget;

		createReportUi( _label, _lastReport );
		_reportLocation->addWidget( _label );
	}
} // namespace VTX::Tool::Mdprep::ui

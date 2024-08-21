#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__
#define __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__

#include <ui/qt/util.hpp>

class QVBoxLayout;

namespace VTX::Util
{
	class Sentry;
	class SentryTarget;
} // namespace VTX::Util
namespace VTX::Tool::Mdprep::Gateway
{
	struct CheckReport;
	struct MdParameters;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class InputChecker;

	inline VTX::UI::QT::Util::LabelWithHelper getWaitingMessage()
	{
		return VTX::UI::QT::Util::LabelWithHelper {
			"Checking System-forcefield compatibility ...",
			"VTX is currently checking if the selected MD Engine support your system with the selected forcefield.",
			VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
		};
	}

	// class responsible for forwarding input checks and writing reports in set location
	class ReportManager
	{
	  public:
		ReportManager( InputChecker );

		// Check inputs and let the user know
		void checkInputs( const Gateway::MdParameters & ) noexcept;

		// Re-generate a report ui item into the new location.
		void resetReportLocation( QVBoxLayout *, VTX::Util::SentryTarget & ) noexcept;

	  private:
		Gateway::CheckReport			   _lastReport;
		InputChecker					   _inputChecker;
		VTX::UI::QT::Util::LabelWithHelper _label;
		QVBoxLayout *					   _reportLocation = nullptr;
		VTX::Util::SentryTarget *		   _sentryTarget   = nullptr;
	};


} // namespace VTX::Tool::Mdprep::ui
#endif

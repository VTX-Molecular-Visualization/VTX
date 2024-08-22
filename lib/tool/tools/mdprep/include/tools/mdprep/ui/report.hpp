#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__
#define __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__

class QVBoxLayout;

namespace VTX::UI::QT::Util
{
	class LabelWithHelper;
}
namespace VTX::Util
{
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

	// class responsible for forwarding input checks and writing reports in set location
	class ReportManager
	{
	  public:
		ReportManager( InputChecker );

		// Check inputs and let the user know
		void checkInputs( const Gateway::MdParameters & ) noexcept;

		// Re-generate a report ui item into the new location. Assumes the old ui elements (if any) have been properly
		// deleted
		void resetReportLocation( QVBoxLayout *, VTX::Util::SentryTarget & ) noexcept;

		struct Data
		{
			Gateway::CheckReport			   report;
			QVBoxLayout *					   target = nullptr;
			VTX::UI::QT::Util::LabelWithHelper label;
		};

	  private:
		Data					  _reportData;
		InputChecker			  _inputChecker;
		VTX::Util::SentryTarget * _sentryTarget = nullptr;
	};

} // namespace VTX::Tool::Mdprep::ui
#endif

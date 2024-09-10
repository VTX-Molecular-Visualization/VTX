#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__
#define __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__

#include <functional>

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
	class UiReportManager;
	class ReportManager;

	struct ReportUi
	{
		VTX::UI::QT::Util::LabelWithHelper content;
		Gateway::CheckReport			   report;
	};
	using UiReportCallback = std::function<void( ReportUi )>;

	/**
	 * @brief Internal class used for UiReport related things
	 */
	class FramedReportManager;

	/**
	 * @brief Class responsible for managing the report area
	 */
	class UiReportManager
	{
		friend ReportManager;

	  public:
		/**
		 * @brief Put the ui part of the report inside the input layout. Only remove the old one if the input is null
		 * @param  Target layout.
		 */
		void relocate( QPointer<QVBoxLayout> ) noexcept;

		/**
		 * @brief Use the input location as a ui target.
		 * @param Manager holding loaction to use.
		 */
		void relocate( UiReportManager & ) noexcept;

		UiReportCallback produceCallback() noexcept;

	  private:
		UiReportManager();
		std::shared_ptr<FramedReportManager> _manager;
	};

	// class responsible for forwarding input checks and writing reports in set location
	class ReportManager
	{
	  public:
		ReportManager( InputChecker );

		/**
		 * @brief Whether a check has already been started on the lifetime of the object.
		 */
		bool hasFirstCheckBeenDone() const noexcept;

		/**
		 * @brief Start a checkinput job, and create a visual representation of the results, and provide it back through
		 * the callback. The callback is guaranteed to be called on the same thread this method is called. The callback
		 * is likely to be called multiple time for process update purposes. The callback shall clean the last item if
		 * any, before adding up the new one.
		 * @param Paramaters to be checked out
		 * @param Callback that'll be called when the report is ready.
		 */
		void checkInputs( const Gateway::MdParameters & ) noexcept;

		/**
		 * @brief Inform the report manager that the resulting report shall be replaced elsewhere
		 * @param  Destination
		 */
		void relocate( QPointer<QVBoxLayout> ) noexcept;

		/**
		 * @brief Use the input location as a ui target.
		 * @param Manager holding loaction to use.
		 */
		void relocate( ReportManager & ) noexcept;

		struct Data
		{
			Gateway::CheckReport report;
			bool				 checkInProgress = false;
		};

	  private:
		UiReportManager _uiManager;
		Data			_reportData;
		InputChecker	_inputChecker;
		bool			firstCheckStarted = false;
	};

} // namespace VTX::Tool::Mdprep::ui
#endif

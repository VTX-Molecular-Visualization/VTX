#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__
#define __VTX_TOOL_TOOLS_MDPREP_UI_REPORT__

#include <util/event_hub.hpp>

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

	/**
	 * @brief class responsible for forwarding input checks and writing reports in set location
	 */
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

	  private:
		struct _Impl;

		std::shared_ptr<_Impl> _impl { nullptr };

		void _receiveReport( Gateway::CheckReport ) noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui
#endif

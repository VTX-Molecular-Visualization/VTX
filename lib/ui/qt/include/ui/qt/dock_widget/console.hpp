#ifndef __VTX_UI_QT_DOCK_WIDGET_CONSOLE__
#define __VTX_UI_QT_DOCK_WIDGET_CONSOLE__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/command_launcher.hpp"
#include <QColor>
#include <QDockWidget>
#include <QListWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <app/vtx_app.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::DockWidget
{
	/**
	 * @brief Max displayed row.
	 */
	constexpr uint CONSOLE_LOG_COUNT = 100;

	/**
	 * @brief Class responsible for docking a console and its prompt.
	 */
	class Console : public BaseDockWidget<Console, 0, 0>
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Console( QWidget * p_parent );
		~Console();

		/**
		 * @brief Add a log row.
		 */
		void log( const VTX::Util::LogInfo & p_logInfo );

		/**
		 * @brief Empty logs.
		 */
		void clear();

	  private:
		/**
		 * @brief Callback id for log printing, to disconnect on destruction.
		 * Useful because logs can be printed after deletion.
		 */
		VTX::Util::CallbackId _onPrintLogCallbackId;

		/**
		 * @brief Display logs.
		 */
		QPointer<QListWidget> _listWidget = nullptr;

		/**
		 * @brief Python command launcher.
		 */
		QPointer<Widget::CommandLauncher> _commandLauncher;
	};

} // namespace VTX::UI::QT::DockWidget

#endif

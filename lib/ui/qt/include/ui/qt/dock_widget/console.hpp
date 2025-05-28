#ifndef __VTX_UI_QT_DOCK_WIDGET_CONSOLE__
#define __VTX_UI_QT_DOCK_WIDGET_CONSOLE__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/widget/command_launcher.hpp"
#include <QColor>
#include <QDockWidget>
#include <QListWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <app/vtx_app.hpp>
#include <mutex>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::DockWidget
{
	class Console;

	/**
	 * @brief Class responsible for docking a console and its prompt. The console will be catching logs and be
	 * displaying some.
	 */
	class Console : public Core::BaseDockWidget<Console, 0, 0>
	{
	  public:
		Console( QWidget * p_parent );
		virtual ~Console() {}

		void clear();
		void scrollToBottom() noexcept;

	  private:
		const int _LOG_COUNT = 500;

		QPointer<QListWidget> _listWidget	   = nullptr;
		std::mutex			  _listWidgetMutex = std::mutex();

		QPointer<Widget::CommandLauncher> _commandLauncher;

		void _appendLog( const ::VTX::Util::LogInfo & p_logInfo );
		void _flush();

		class ScrollToBottomFilter : public QObject
		{
		  public:
			ScrollToBottomFilter( Console & );

			virtual bool eventFilter( QObject * object, QEvent * event ) override;

		  private:
			Console * _console = nullptr;
		};
		QPointer<ScrollToBottomFilter> _filter { new ScrollToBottomFilter( *this ) };
	};

} // namespace VTX::UI::QT::DockWidget

#endif

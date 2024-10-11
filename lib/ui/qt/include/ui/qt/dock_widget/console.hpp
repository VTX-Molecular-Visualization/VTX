#ifndef __VTX_UI_QT_DOCK_WIDGET_CONSOLE__
#define __VTX_UI_QT_DOCK_WIDGET_CONSOLE__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/line_edit/command_launcher.hpp"
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

	class Console : public Core::BaseDockWidget<Console>
	{
	  public:
		Console( QWidget * p_parent );
		virtual ~Console() {}

		void clear();

	  private:
		const int _LOG_COUNT = 500;

		QListWidget * _listWidget	   = nullptr;
		std::mutex	  _listWidgetMutex = std::mutex();

		QPointer<LineEdit::CommandLauncher> _commandLauncher;

		void _appendLog( const Util::LogInfo & p_logInfo );
		void _flush();
	};

} // namespace VTX::UI::QT::DockWidget

#endif

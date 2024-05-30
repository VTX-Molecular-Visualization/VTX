#ifndef __VTX_UI_TOOL_CONSOLE_WIDGET_CONSOLE__
#define __VTX_UI_TOOL_CONSOLE_WIDGET_CONSOLE__

#include "ui/qt/qt_panel.hpp"
#include "ui/qt/widget/pytx/line_edit.hpp"
#include <QColor>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QSize>
#include <mutex>
#include <string>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget::Console
{
	class Panel : public QtDockablePanel
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		// TODO: move to style.
		inline static const QColor CONSOLE_INFO_COLOR	  = QColor( "white" );
		inline static const QColor CONSOLE_DEBUG_COLOR	  = QColor( "white" );
		inline static const QColor CONSOLE_WARNING_COLOR  = QColor( "yellow" );
		inline static const QColor CONSOLE_ERROR_COLOR	  = QColor( "red" );
		inline static const QColor CONSOLE_CRITICAL_COLOR = QColor( "red" );

		inline static const QSize CONSOLE_MINIMUM_SIZE	 = QSize( 1, 1 );
		inline static const QSize CONSOLE_PREFERRED_SIZE = QSize( 100, 80 );

	  private:
		inline static const int _LOG_COUNT = 50;

	  public:
		void clearLogs();
		void localize() override;

	  protected:
		Panel( QWidget * p_parent = nullptr );
		~Panel() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		using CommandLinePrompt = Widget::PyTX::CommandLinePrompt;

		// Logs.
		QListWidget * _listWidget	   = nullptr;
		std::mutex	  _listWidgetMutex = std::mutex();

		void   _appendLog( const Util::LogInfo & p_logInfo );
		void   _flush();
		void   _clearConsoleAction();
		QColor _getMessageColor( const Util::LOG_LEVEL p_level );

		// Command line.
		CommandLinePrompt * _promptWidget = nullptr;

		void _setupCompleter();
		void _launchCommand();
	};
} // namespace VTX::UI::QT::Widget::Console
#endif

#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "settings.hpp"
#include "widget/main_window.hpp"
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include <app/vtx_app.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{
	class Application final : public QApplication, public App::VTXApp
	{
	  public:
		Application( const App::Args & );
		~Application();

		void start() override;
		void stop();

		// bool event( QEvent * ) override;
		//  Check exception in Qt events.
		bool notify( QObject * const, QEvent * const ) override;

	  private:
		QPointer<Widget::MainWindow> _mainWindow;
		QTimer						 _timer;
		VTX::Util::Chrono			 _durationTimer;

		void _loadTheme();
	};
} // namespace VTX::UI::QT

#endif
